#include "libavutil/terminal.h"
#include <stdio.h>

void log_oom() {
  printf(CSI_COLOR_RED "Fatal error: Out of memory\n" CSI_COLOR_RESET);
}
