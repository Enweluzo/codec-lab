#include "libavutil/map.h"
#include "libavutil/terminal.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

void map_test() {
  Map *map = create_map(10);
  assert(map != NULL);

  char *key1 = "kelvin";
  char *value1 = "caleb";

  char *key2 = "language";
  char *value2 = "C";

  char *key3 = "data_structure";
  char *value3 = "hash_map";

  map_insert(key1, value1, strlen(key1), map);
  map_insert(key2, value2, strlen(key2), map);
  map_insert(key3, value3, strlen(key3), map);

  char *result1 = map_get(key1, strlen(key1), map);
  char *result2 = map_get(key2, strlen(key2), map);
  char *result3 = map_get(key3, strlen(key3), map);

  // Verify missing keys.
  char *missing_key = "does_not_exist";
  char *result4 = map_get(missing_key, strlen(missing_key), map);
  destroy_map(map);

  bool passed1 = result1 != NULL && strcmp(result1, value1) == 0;
  bool passed2 = result2 != NULL && strcmp(result2, value2) == 0;
  bool passed3 = result3 != NULL && strcmp(result3, value3) == 0;
  bool passed4 = result4 == NULL;

  bool test_passed = passed1 && passed2 && passed3 && passed4;

  if (test_passed) {
    printf(CSI_COLOR_GREEN "SUCCESS" CSI_COLOR_RESET ": Hashmap\n");
  } else {
    printf(CSI_COLOR_RED "FAILED" CSI_COLOR_RESET ": Hashmap\n");
  }
}
