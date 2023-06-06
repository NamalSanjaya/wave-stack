#include <stdio.h>
#include <math.h>
#include <stddef.h>
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <stdint.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// header file to communicate with stack
#include <libwave_sock.h>

#define WAVE_SCKFILE "/home/suhail/workspace2/fyp/wave-stack/test/bin/wave_sckfile"

#define DATABASE_URL "https://first-app-89614-default-rtdb.firebaseio.com/"
#define NODE_PATH "/locations.json"

#define EARTH_RADIUS 6371 // Earth's radius in km

#define THERSHOLD 10
#define MUL_FACTOR 1e7

struct Entity {
    double latitude;
    double longitude;
    double speed;
};

double vincenty(double lat1, double lon1, double lat2, double lon2) ;
void callback(local_resp_t *resp);

int main() {

    int sock_fd = wave_sock_open(WAVE_SCKFILE);
    if(sock_fd == -1) return 1;
    wave_sock_listen(sock_fd, callback);

    return 0;
}

void callback(local_resp_t *resp){

    uint32_t lat = 0;
    uint32_t longt = 0;

    // Extract the first 32 bits
    for (int i = 0; i < 4; i++) {
        lat |= ( (uint32_t) *((resp->buf)+i) << (24 - (8 * i)));
    }

    // Extract the second 32 bits
    for (int i = 4; i < 8; i++) {
        longt |= ( (uint32_t) *((resp->buf)+i) << (24 - (8 * (i - 4))));
    }

    double lat_2d = lat/MUL_FACTOR;
    double longt_2d = longt/MUL_FACTOR;

    // Replacing this with firebase reading
    // double rx_fix_lat = 6.7967400;
    // double rx_fix_longt = 79.9012834;

    // Reading rx coordiantes from firebase server
    CURL *curl;
    CURLcode res;
    char url[256];
    char data[1024];

    // Initialize CURL
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        // Create the URL to fetch data from the Firebase Realtime Database
        snprintf(url, sizeof(url), "%s%s", DATABASE_URL, NODE_PATH);

        // Set CURL options
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);

        // Make the request
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "Failed to fetch data: %s\n", curl_easy_strerror(res));
        } else {
            // Parse JSON data
            cJSON *root = cJSON_Parse(data);
            if (root) {
                // Get the latitude, longitude, and speed values
                cJSON *latitudeNode = cJSON_GetObjectItemCaseSensitive(root, "latitude");
                cJSON *longitudeNode = cJSON_GetObjectItemCaseSensitive(root, "longitude");
                cJSON *speedNode = cJSON_GetObjectItemCaseSensitive(root, "speed");

                if (cJSON_IsNumber(latitudeNode) && cJSON_IsNumber(longitudeNode) && cJSON_IsNumber(speedNode)) {
                    // TODO: change data type to doulbe(store 64bits for each). Think about this
                    double rx_latitude  =  latitudeNode->valuedouble;
                    double rx_longitude =  longitudeNode->valuedouble;
                    // int speed = speedNode->valueint;

                    struct Entity entity1 = {rx_latitude, rx_longitude, 10}; // Rx fix coordinates
                    struct Entity entity2 = {lat_2d, longt_2d, 20}; // moving TX coordinates

                    double distance = vincenty(entity1.latitude, entity1.longitude,
                                                entity2.latitude, entity2.longitude);
                    /*
                        double time_to_collision = distance / (entity2.speed + entity1.speed);
                    */
                    printf("Distance: %f\n", distance);
                    if (distance < THERSHOLD) { // predefined threshold
                        printf("Collision detected!\n");
                        usleep(50000);
                    } else {
                        printf("No collision detected.\n");
                    }
                    printf("-------------------------------------------------------\n");
                } else {
                    printf("Latitude, longitude, or speed not found or invalid\n");
                }

                // Cleanup cJSON
                cJSON_Delete(root);
            } else {
                printf("Failed to parse JSON\n");
            }
        }

        // Cleanup CURL
        curl_easy_cleanup(curl);
    }

    // Cleanup CURL global state
    curl_global_cleanup();
}

// vincenty algorithm to find distance between two gps points 
double vincenty(double lat1, double lon1, double lat2, double lon2) {
    // WGS-84 ellipsoid constants
    double a = 6378137.0;
    double f = 1/298.257223563;
    double pi = M_PI;
    // Convert latitude and longitude from decimal degrees to radians
    double phi1 = lat1 * pi / 180.0;
    double lambda1 = lon1 * pi / 180.0;
    double phi2 = lat2 * pi / 180.0;
    double lambda2 = lon2 * pi / 180.0;

    // Calculate the distance using the Vincenty formula
    double L = lambda2 - lambda1;
    double tan_u1 = (1 - f) * tan(phi1);
    double cos_u1 = 1 / sqrt((1 + tan_u1*tan_u1));
    double sin_u1 = tan_u1 * cos_u1;
    double tan_u2 = (1 - f) * tan(phi2);
    double cos_u2 = 1 / sqrt((1 + tan_u2*tan_u2));
    double sin_u2 = tan_u2 * cos_u2;
    double sigma = 0;
    double delta_sigma = 2 * pi;
    while (fabs(delta_sigma) > 1e-12) {
        double sin_sigma = sqrt((cos_u2 * sin(L)) * (cos_u2 * sin(L)) +
                         (cos_u1 * sin_u2 - sin_u1 * cos_u2 * cos(L)) * (cos_u1 * sin_u2 - sin_u1 * cos_u2 * cos(L)));
        double cos_sigma = sin_u1 * sin_u2 + cos_u1 * cos_u2 * cos(L);
        double sigma_prime = sigma;
        sigma = atan2(sin_sigma, cos_sigma);
        double sin_alpha = (cos_u1 * cos_u2 * sin(L)) / sin_sigma;
        double cos_sq_alpha = 1 - sin_alpha*sin_alpha;
        double cos_2sigma_m = cos_sigma - (2 * sin_u1 * sin_u2 / cos_sq_alpha);
        double C = f / 16 * cos_sq_alpha * (4 + f * (4 - 3 * cos_sq_alpha));
        double delta_L = L;
        L = (lambda2 - lambda1) + (1 - C) * f * sin_alpha * (sigma + C * sin_sigma *
                                                             (cos_2sigma_m + C * cos_sigma *
                                                              (-1 + 2 * cos_2sigma_m*cos_2sigma_m)));
        delta_sigma = sigma - sigma_prime;
    }

    // Calculate the distance in meters
    double d = a * sigma;
    return d;
}