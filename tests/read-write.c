
#include "atomic/nucleus.h"

void _start(void) {
  char buf[8];
  _atomic_read(0, buf, 8);
  _atomic_exit(0);
}
