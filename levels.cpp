#pragma leco add_resource "levels.dat"
module game;
import fork;
import hai;
import jute;
import silog;
import sires;

namespace sg = sokoban::game;
namespace sl = sokoban::levels;

static constexpr const auto max_level_capacity = 500;

const unsigned sl::level_width = 32;
const unsigned sl::level_height = 24;

static unsigned g_cur_level{};
static unsigned g_max_level{};
static char g_data[max_level_capacity][sg::max_quads]{};

mno::req<void> read_level(yoyo::subreader data) {
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

void sl::read_levels() {
  sires::open("levels.dat")
      .fpeek(frk::assert("SKB"))
      .fpeek(frk::take_all("LEVL", read_level))
      .map(frk::end())
      .trace("loading levels")
      .log_error();
}

unsigned sl::current_level() { return g_cur_level; }
unsigned sl::max_levels() { return g_max_level + 1; }
jute::view sl::level(unsigned l) { return g_data[l]; }
void sl::load_level(unsigned l) { sl::load_level(g_data[l], l); }
void sl::load_level(jute::view lvl, unsigned lvl_number) {
  // TODO: assert lvl is smaller than our buffer
  game::player_pos = levels::level_quad_count();
  for (auto i = 0U; i < levels::level_quad_count(); i++) {
    switch (auto c = lvl[i]) {
      using namespace spr;
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
  g_cur_level = lvl_number;
}
