#include <stdio.h>
#include <math.h>
#include <stddef.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// header file to communicate with stack
#include <libwave_sock.h>

#define SCKFILE "/home/sdrns/workspace/fyp_work/layer_development/wave_stack/test/bin/sckfile"
#define WAVE_SCKFILE "/home/sdrns/workspace/fyp_work/layer_development/wave_stack/test/bin/wave_sckfile"

#define EARTH_RADIUS 6371 // Earth's radius in km

struct Entity {
    double latitude;
    double longitude;
    double speed;
};

double vincenty(double lat1, double lon1, double lat2, double lon2) ;
void action_callback(local_resp_t *resp);

int main() {

    int sock_fd = wave_sock_open(WAVE_SCKFILE);
    if(sock_fd == -1) return 1;
    wave_sock_listen(sock_fd, action_callback);

    return 0;
}

void action_callback(local_resp_t *resp){
    print_binx_temp(resp->buf, resp->data_size);
    printf("\ndata-size: %ld...\n", resp->data_size);
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
