#include "expect.h"
#include "lio.h"
#include <stdint.h>
#include <string.h>

int expect_key(struct lio_reader *x, char const *key)
{
  uint32_t size = 0;
  unsigned char buf[16] = {0};

  unsigned char *ptr = NULL;
  if (lio_read(x, &ptr)) return 1;
  if (lio_free(x, lip_unpack_string(ptr, &size))) return 1;
  if ((size_t)size > sizeof(buf)) return 1;

  if (lio_readb(x, size, buf)) return 1;
  if (size != (uint32_t)strlen(key)) return 1;
  return memcmp(key, buf, size) != 0;
}
