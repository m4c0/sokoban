#pragma leco tool
#define _CRT_SECURE_NO_WARNINGS

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

import fork;
import hai;
import traits;
import yoyo;

int main() {
  auto f = fopen("levels.dat", "rb");
  fseek(f, 8, SEEK_SET);

  hai::array<hai::array<char>> levels{60};

  while (!feof(f)) {
    unsigned tag{};
    fread(&tag, 4, 1, f);
    if (feof(f))
      break;
    assert(tag == 'LEVL');

    unsigned len{};
    fread(&len, 4, 1, f);

    hai::array<char> buf{len};
    fread(buf.begin(), len, 1, f);

    auto lvl = *reinterpret_cast<int *>(buf.begin());
    levels[lvl] = traits::move(buf);
  }

  fclose(f);

  auto res = yoyo::file_writer::open("levels.dat").fpeek(frk::signature("SKB"));
  for (auto &lvl : levels) {
    res = res.fpeek(frk::chunk("LEVL", lvl.begin(), lvl.size()));
  }
  res.map(frk::end()).log_error();
}
