#include <string.h>
#include <stdlib.h>

#include "power.h"

#ifdef COMPILE_WITH_SENSORS

#include <sensors/sensors.h>
#include <sensors/error.h>
#define MAX_CHIPS   16
#define MAX_SENSORS MAX_CHIPS * 8
// sensors_chip_name *chips[MAX_CHIPS] = { 0 };
// Here, we only care about TEMP feature, and inside it TEMP_INPUT subfeature
// sensors_feature_type == SENSORS_FEATURE_TEMP
// sensors_subfeature_type == SENSORS_SUBFEATURE_TEMP_INPUT

typedef struct {
  sensors_chip_name chip_name;
  sensors_subfeature subfeature;
} temp_sensor_t;

temp_sensor_t temp_sensors[MAX_SENSORS] = { 0 };
double temp_values[MAX_SENSORS];
int nb_temp_sensors = 0;

void my_init_sensors() {
  sensors_init(NULL);
  // Get chips
  const sensors_chip_name *chip_name;
	int chip_nr = 0;
  while ( chip_name = sensors_get_detected_chips(NULL, &chip_nr) ) {
    int ret_feature = 0, ret_subfeature = 0;
    const sensors_feature *feature;
    const sensors_subfeature *subfeature;
    double val;

    while ((feature = sensors_get_features(chip_name, &ret_feature))) {
      if ( feature->type == SENSORS_FEATURE_TEMP ) {
        ret_subfeature = 0;
        while ( subfeature = sensors_get_all_subfeatures(chip_name, feature, &ret_subfeature) ) {
          if ( subfeature->flags & SENSORS_MODE_R && subfeature->type == SENSORS_SUBFEATURE_TEMP_INPUT ) {
            memcpy(&temp_sensors[nb_temp_sensors].chip_name, chip_name, sizeof(sensors_chip_name));
            memcpy(&temp_sensors[nb_temp_sensors].subfeature, subfeature, sizeof(sensors_subfeature));
//            temp_sensors[nb_temp_sensors].subfeat_nr = subfeature->number;
            nb_temp_sensors++;
          }
        }
      }
    }
  }
}

void print_temp_values(FILE *file, int ipmi_watt, double time) {
  
  int i, err;
  double val;
  
  fprintf(file, "%f %d ", time, ipmi_watt);
  for (i = 0; i < nb_temp_sensors; i++) {
    if ((err = sensors_get_value(&temp_sensors[i].chip_name, temp_sensors[i].subfeature.number, &val)))
    fprintf(stderr, "ERROR: Can't get value of subfeature %s: %s\n", temp_sensors[i].subfeature.name, sensors_strerror(err));
    else
    fprintf(file, "%.0f ", val);
//    temp_values[i] = val;
//    printf("  %s: %.3f\n", temp_sensors[i].subfeature.name, val);
  }
  fprintf(file, "\n");
}
#else
void print_temp_values(FILE *file, int ipmi_watt, double time) {
  fprintf(file, "%f %d 0\n", time, ipmi_watt);
}
void my_init_sensors() {
}
#endif
