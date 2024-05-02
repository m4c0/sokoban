#pragma leco app
#pragma leco add_impl splay
export module sokoban;
import casein;
import game;

namespace sa = sokoban::audio;
namespace sg = sokoban::game;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;
using namespace sokoban::enums;

static struct : sr::rnd {
  const char *app_name() const noexcept override { return "sokoban"; }
} r;

static sa::streamer audio{};

void setup_game(int level);

struct init {
  init() { setup_game(0); }
} i;
