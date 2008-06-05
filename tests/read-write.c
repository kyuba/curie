
#include "atomic/nucleus.h"

int atomic_main(void) {
  char buf[8];
  (void)_atomic_read(0, buf, 8);
  return (0);
}
