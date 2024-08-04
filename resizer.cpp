#pragma leco tool
import fork;
import hai;
import silog;
import yoyo;

static constexpr const auto old_width = 24;
static constexpr const auto old_height = 20;
static constexpr const auto old_size = old_width * old_height;

static constexpr const auto new_width = 32;
static constexpr const auto new_height = 24;
static constexpr const auto new_size = new_width * new_height;

struct level {
  unsigned id;
  char data[new_size];
};
hai::varray<level> g_data{1000};

static mno::req<void> read_level(yoyo::subreader data) {
  level lvl{};
  for (auto &c : lvl.data)
    c = ' ';

  char old[1024];

  constexpr const auto dw = (new_width - old_width) / 2;
  constexpr const auto dh = (new_height - old_height) / 2;
  return data.read_u32()
      .fmap([&](auto l) {
        lvl.id = l;
        return data.read(old, old_size);
      })
      .map([&] {
        auto op = old;
        for (auto y = 0; y < old_height; y++) {
          auto np = lvl.data + (y + dh) * new_width + dw;
          for (auto x = 0; x < old_width; x++, np++, op++) {
            *np = *op;
          }
        }
      })
      .map([&] { g_data.push_back(lvl); });
}

int main() {
  auto res = yoyo::file_reader::open("levels.dat")
                 .fpeek(frk::assert("SKB"))
                 .fpeek(frk::take_all("LEVL", read_level))
                 .map(frk::end())
                 .fmap([] { return yoyo::file_writer::open("levels.dat"); })
                 .fpeek(frk::signature("SKB"));

  for (auto &lvl : g_data) {
    res = res.fpeek(frk::chunk("LEVL", lvl));
  }

  return res.map([](auto &) { return 0; }).log_error([] { return 1; });
}
