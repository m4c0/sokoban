#include <assert.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
  uint8_t sz[4];
  uint32_t id;
  uint32_t lvl;
} frk_hdr_t;

int main() {
  FILE * f = fopen("../levels.dat", "rb");
  assert(f);
  FILE * o = fopen("levels.txt", "wb");
  assert(o);

  uint64_t id;
  assert(1 == fread(&id, 8, 1, f));
  assert(id == 0x0A1A0A0D424B5389l);

  while (!feof(f)) {
    frk_hdr_t hdr;
    if (1 != fread(&hdr, 12, 1, f)) {
      assert(feof(f));
      continue;
    }
    assert(hdr.id == 'LVEL');

    uint32_t sz = (hdr.sz[0] << 24) | (hdr.sz[1] << 16) | (hdr.sz[2] << 8) | hdr.sz[3];

    char buf[10240] = {0};
    assert(1 == fread(buf, sz - 4, 1, f));
    uint32_t crc;
    assert(1 == fread(&crc, 4, 1, f));

    fprintf(o, "|%s|\n", buf);
  }

  fclose(f);
  fclose(o);
}
