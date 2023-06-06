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
#include <gtk/gtk.h>
#include <pthread.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// header file to communicate with stack
#include <libwave_sock.h>

#define WAVE_SCKFILE "/home/suhail/workspace2/fyp/wave-stack/test/bin/wave_sckfile"

#define DATABASE_URL "https://first-app-89614-default-rtdb.firebaseio.com/"
#define NODE_PATH "/locations.json"
#define UPDATE_INTERVAL_MS 100

#define EARTH_RADIUS 6371 // Earth's radius in km

#define THERSHOLD 10
#define MUL_FACTOR 1e7

struct Entity {
    double latitude;
    double longitude;
    double speed;
};

// Global variables
GtkWidget *window;
GtkWidget *label;
GtkWidget *image;
pthread_t fetch_thread;
gboolean fetch_data_pending = FALSE;
int state = 0;
float y;

double vincenty(double lat1, double lon1, double lat2, double lon2) ;
void callback(local_resp_t *resp);
size_t write_callback(void *ptr, size_t size, size_t nmemb, char *data);
void *run_gui();
void update_label_text(const char *text);
void update_image();
void window_destroy(GtkWidget *widget, gpointer data);
gboolean window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
void *fetch_data(void *data);
gboolean update_label(gpointer data);

int main() {

    pthread_t gui_th;
    if(pthread_create(&gui_th, NULL, &run_gui, NULL) != 0) return 1;

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
                        state = 1;
                        update_image();
                    } else {
                        printf("No collision detected.\n");
                        state = 0;
                        update_image();
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

// Callback function for receiving data
size_t write_callback(void *ptr, size_t size, size_t nmemb, char *data) {
    memcpy(data, ptr, size * nmemb);
    return size * nmemb;
}

// if state zero , y - 0.63 ,  state 1 y-2.2

// Function to update the label text with the received data
void update_label_text(const char *text) {
    if (GTK_IS_LABEL(label)) {
        gtk_label_set_text(GTK_LABEL(label), text);
        
        // Increase font size to 200%
        PangoFontDescription *font_desc = pango_font_description_from_string("Sans Bold 25");
        gtk_widget_override_font(label, font_desc);
        pango_font_description_free(font_desc);
    }
}

// Function to update the state variable and show/hide the image
void update_image() {
    if (state) {
        gtk_widget_show(image); // Show the image when state is 1
    } else {
        gtk_widget_hide(image); // Hide the image when state is 0
    }
}

// Callback function for the "destroy" signal of the window
void window_destroy(GtkWidget *widget, gpointer data) {
    // Cancel the fetch thread if it's running
    if (fetch_data_pending) {
        pthread_cancel(fetch_thread);
        fetch_data_pending = FALSE;
    }
    gtk_main_quit();
}

// Callback function for the "delete-event" signal of the window
gboolean window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data) {
    return FALSE; // Propagate the event further for default handling
}

// Function to fetch data from the Firebase Realtime Database
void *fetch_data(void *data) {
    CURL *curl;
    CURLcode res;
    char url[256];
    char fetch_data[1024];

    // Initialize CURL
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        // Create the URL to fetch data from the Firebase Realtime Database
        snprintf(url, sizeof(url), "%s%s", DATABASE_URL, NODE_PATH);

        // Set CURL options
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fetch_data);

        // Make the request
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "Failed to fetch data: %s\n", curl_easy_strerror(res));
        } else {
            // Process the received data
            cJSON *root = cJSON_Parse(fetch_data);
            if (root) {
                // Get the latitude and longitude values
                cJSON *latitudeNode = cJSON_GetObjectItemCaseSensitive(root, "latitude");
                cJSON *longitudeNode = cJSON_GetObjectItemCaseSensitive(root, "longitude");

                if (cJSON_IsNumber(latitudeNode) && cJSON_IsNumber(longitudeNode)) {
                    double latitude = latitudeNode->valuedouble;
                    double longitude = longitudeNode->valuedouble;

                    // Format the text to display in the label
                    char text[100];
                    snprintf(text, sizeof(text), "Latitude: %.7f; Longitude: %.7f", latitude, longitude);

                    // Update the label text
                    g_idle_add((GSourceFunc)update_label_text, g_strdup(text));
                } else {
                    fprintf(stderr, "Latitude or longitude not found or invalid\n");
                }

                // Cleanup cJSON
                cJSON_Delete(root);
            } else {
                fprintf(stderr, "Failed to parse JSON\n");
            }
        }

        // Cleanup CURL
        curl_easy_cleanup(curl);
    }

    // Cleanup CURL global state
    curl_global_cleanup();

    fetch_data_pending = FALSE;

    return NULL;
}

// Function to update the label text periodically
gboolean update_label(gpointer data) {
    // Check if a fetch request is already pending
    if (fetch_data_pending) {
        return G_SOURCE_CONTINUE;
    }

    fetch_data_pending = TRUE;

    // Start the fetch_data thread
    pthread_create(&fetch_thread, NULL, fetch_data, NULL);

    // Return G_SOURCE_CONTINUE to continue updating the label periodically
    return G_SOURCE_CONTINUE;
}


void *run_gui(){
    // Initialize GTK+
    gtk_init(NULL, NULL);
    if (state == 0) {
        y = 0.58;
    } else if (state == 1) {
        y = 2.2;
    } else {
        printf("Invalid state value\n");
        return NULL;
    }

    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Lamborghini Veneno");
    g_signal_connect(window, "destroy", G_CALLBACK(window_destroy), NULL);
    g_signal_connect(window, "delete-event", G_CALLBACK(window_delete_event), NULL);

    // Create a vertical box container
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    // Create a GtkCssProvider for setting the CSS style
    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(cssProvider,
        "window {\n"
        "   background-image: url('Background.png');\n" // Replace 'background.png' with your background image file path
        "   background-size: contain;\n" // Adjust the background-size property to 'contain'
        "}\n", -1, NULL);

    // Apply the CSS style to the window
    GtkStyleContext *styleContext = gtk_widget_get_style_context(window);
    gtk_style_context_add_provider(styleContext, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Create an alignment widget for centering the label
    GtkWidget *alignment = gtk_alignment_new(0.305, y , 0, 0);
    gtk_box_pack_start(GTK_BOX(box), alignment, TRUE, TRUE, 0);

    // Create a label widget
    label = gtk_label_new(NULL);
    gtk_container_add(GTK_CONTAINER(alignment), label);

    // Create an alignment widget and add it to the box
    GtkWidget *imageAlignment = gtk_alignment_new(0.35, 0.6, 0, 0);
    gtk_box_pack_start(GTK_BOX(box), imageAlignment, TRUE, TRUE, 0);

    // Create an image widget
    image = gtk_image_new_from_file("main.png"); // Replace "main.png" with your image file path
    gtk_widget_set_size_request(image, 200, 200); // Set the size of the image
    gtk_container_add(GTK_CONTAINER(imageAlignment), image);

    // Show all the widgets
    gtk_widget_show_all(window);

    // Start updating the label periodically with a debounce mechanism
    guint update_interval = UPDATE_INTERVAL_MS;
    g_timeout_add(update_interval, (GSourceFunc)update_label, NULL);

    // Update the image based onz the initial state
    update_image();

    // Run the GTK main loop
    gtk_main();

    return NULL;
}
