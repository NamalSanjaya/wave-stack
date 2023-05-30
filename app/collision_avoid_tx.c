#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <unistd.h> 
#include <stdint.h>
#include <time.h>

// header file to communicate with wave stack
#include <libwave_sock.h>

#define SCKFILE "/home/sdrns/workspace/fyp_work/layer_development/wave_stack/test/bin/sckfile"

#define DATABASE_URL "https://first-app-89614-default-rtdb.firebaseio.com/"
#define NODE_PATH "/locations.json"
#define UPDATE_INTERVAL_MS 100

size_t write_callback(void *ptr, size_t size, size_t nmemb, char *data);
int app_send_wsm(double lat, double longt);
void store8(uint8_t *buf,uint8_t *i, uint8_t data);
void store32(uint8_t *buf,uint8_t *i, uint32_t data);
void store64(uint8_t *buf,uint8_t *i, uint64_t data);

int main() {
    CURL *curl;
    CURLcode res;
    char url[256];
    char data[1024];

    // Initialize CURL
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        while (1) {
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
                        double latitude  =  latitudeNode->valuedouble;
                        double longitude =  longitudeNode->valuedouble;
                        // int speed = speedNode->valueint;
                        
                        /**
                         * 1. create wsm with lat, longt
                         * 2. send it to wave stack
                         */
                        app_send_wsm(latitude, longitude);

                        printf("Latitude: %f\n", latitude);
                        printf("Longitude: %f\n", longitude);
                        // printf("Speed: %d\n", speed);
                    } else {
                        printf("Latitude, longitude, or speed not found or invalid\n");
                    }

                    // Cleanup cJSON
                    cJSON_Delete(root);
                } else {
                    printf("Failed to parse JSON\n");
                }
            }

            // Wait for the specified interval
            usleep(UPDATE_INTERVAL_MS * 1000);
        }

        // Cleanup CURL
        curl_easy_cleanup(curl);
    }

    // Cleanup CURL global state
    curl_global_cleanup();
    return 0;
}

// Callback function for receiving data
size_t write_callback(void *ptr, size_t size, size_t nmemb, char *data) {
    memcpy(data, ptr, size * nmemb);
    return size * nmemb;
}

// Communicate with stack
int app_send_wsm(double lat, double longt){
    uint8_t chan_id = 172; 
    enum time_slot timeslot = time_slot1; 
    uint8_t data_rate = 67; 
    int8_t tx_power = -10; 
    uint8_t channel_load = 5; 
    uint8_t info_elem_indicator = 1; 
    uint8_t prority = 1; 
    uint64_t wsm_expire_time = 10; 

    // only for two 64-bits fields
    uint16_t len = 128; 

    uint64_t loc2d_lat, loc2d_longt;
    loc2d_lat = lat;
    loc2d_longt = longt;

    uint8_t *data = (uint8_t *) calloc(len, sizeof(uint8_t));

    uint8_t indx = 0;
    store64(data, &indx, loc2d_lat);
    store64(data, &indx, loc2d_longt);

    uint8_t peer_mac_addr[6] =  {255, 255, 255, 255, 255, 255}; 
    uint32_t psid = 0xC00305;

    int8_t st = app_wsm_waveshortmsg_req(chan_id, timeslot,  data_rate, tx_power, channel_load, info_elem_indicator, prority, 
        wsm_expire_time, len, data, peer_mac_addr, psid, SCKFILE);

    // print_binx(data, len);
    if (st<0){
        printf("something went wrong...\n");
        return 1;
    }
    printf("--done--\n");
    return 0;
}

void store8(uint8_t *buf,uint8_t *i, uint8_t data){
    buf[*i] = data;
    (*i)++;
}

void store32(uint8_t *buf,uint8_t *i, uint32_t data){
    store8(buf, i, data >> 24);
    store8(buf, i, data >> 16);
    store8(buf, i, data >> 8);
    store8(buf, i, data);
}

void store64(uint8_t *buf,uint8_t *i, uint64_t data){
    store8(buf, i, data >> 56);
    store8(buf, i, data >> 48);
    store8(buf, i, data >> 40);
    store8(buf, i, data >> 32);
    store8(buf, i, data >> 24);
    store8(buf, i, data >> 16);
    store8(buf, i, data >> 8);
    store8(buf, i, data);
}
