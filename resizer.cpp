#pragma leco tool
import fork;
import hai;
import silog;
import yoyo;

static constexpr const auto old_size = 24 * 14;
static constexpr const auto new_size = 24 * 20;

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

  constexpr const auto a = (new_size - old_size) / 2;
  return data.read_u32()
      .fmap([&](auto l) {
        lvl.id = l;
        return data.read(lvl.data + a, old_size);
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
static void level_dump() {
  yoyo::file_writer w{"levels.dat"};
  frk::push('SKBN', &w, [&](auto w) {
    return store_levels(g_data.begin(), g_data.end(), w);
  }).take([](auto err) {
    silog::log(silog::error, "failed to write levels: %s", err);
  });
}

int main() {
  yoyo::file_reader::open("levels.dat")
      .fmap([](auto &&r) { return frk::read(&r).fmap(read_list); })
      .take([](auto msg) {
        silog::log(silog::error, "failed to load levels data: %s", msg);
        throw 0;
      });
  level_dump();
}
