#pragma leco add_resource "levels.dat"
module game;
import fork;
import jute;
import silog;
import sires;

namespace sl = sokoban::levels;

static constexpr const auto max_level_capacity = 500;

const unsigned sl::level_width = 24;
const unsigned sl::level_height = 14;

static unsigned g_cur_level{};
static unsigned g_max_level{};
hai::array<char[1024]> g_data{max_level_capacity};

mno::req<void> read_level(frk::pair p) {
  auto [fourcc, data] = p;
  if (fourcc != 'LEVL')
    return {};

  return data.read_u32()
      .assert([](auto lvl) { return lvl <= max_level_capacity; },
              "max level capacity")
      .map([&](auto lvl) {
        g_max_level = g_max_level < lvl ? lvl : g_max_level;
        return g_data[lvl];
      })
      .fmap([&](auto *d) {
        return data.read(d, sl::level_width * sl::level_height);
      });
}
mno::req<void> read_list(frk::pair p) {
  auto [fourcc, data] = p;
  if (fourcc != 'SKBN') {
    silog::log(silog::error, "data file is not valid");
    return {};
  }
  return frk::read_list(&data, read_level);
}

static struct lvls {
  lvls() {
    sires::open("levels.dat")
        .fmap([](auto &&r) { return frk::read(&*r).fmap(read_list); })
        .take([](auto msg) {
          silog::log(silog::error, "failed to load levels data: %s", msg);
        });
  }
} g_lvls;

unsigned sl::current_level() { return g_cur_level + 1; }
unsigned sl::max_levels() { return g_max_level + 1; }
jute::view sl::level(unsigned l) { return g_data[l]; }
void sl::load_level(unsigned l) { sl::load_level(g_data[l], l); }
void sl::load_level(jute::view lvl, unsigned lvl_number) {
  // TODO: assert lvl is smaller than our buffer
  for (auto i = 0U; i < levels::level_quad_count(); i++) {
    switch (auto c = lvl[i]) {
      using namespace enums;
    case player:
      game::grid[i] = empty;
      game::player_pos = i;
      break;
    case player_target:
      game::grid[i] = target;
      game::player_pos = i;
      break;
    default:
      game::grid[i] = static_cast<blocks>(c);
      break;
    }
  }
}
