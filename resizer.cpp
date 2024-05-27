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
  char data[1024];
};
hai::varray<level> g_data{1000};

mno::req<void> read_level(frk::pair p) {
  auto [fourcc, data] = p;
  if (fourcc != 'LEVL')
    return {};

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
mno::req<void> read_list(frk::pair p) {
  auto [fourcc, data] = p;
  if (fourcc != 'SKBN') {
    silog::log(silog::error, "data file is not valid");
    return {};
  }
  return frk::read_list(&data, read_level);
}

static mno::req<void> store_level(level &l, yoyo::writer *w) {
  return w->write_u32(l.id).fmap([&] { return w->write(l.data, new_size); });
}
static mno::req<void> store_levels(level *l, level *end, yoyo::writer *w) {
  if (l == end) {
    return {};
  }
  return frk::push('LEVL', w, [&](auto) { return store_level(*l, w); })
      .fmap([&] { return store_levels(l + 1, end, w); });
}
static mno::req<void> level_dump() {
  return yoyo::file_writer::open("levels.dat")
      .fmap([](auto &&w) {
        return frk::push('SKBN', &w, [&](auto w) {
          return store_levels(g_data.begin(), g_data.end(), w);
        });
      })
      .trace("writing levels");
}

int main() {
  bool success =
      yoyo::file_reader::open("levels.dat")
          .fmap([](auto &&r) { return frk::read(&r).fmap(read_list); })
          .trace("loading levels")
          .fmap(level_dump)
          .map([] { return true; })
          .log_error();
  return success ? 0 : 1;
}
