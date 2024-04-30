module game;
import siaudio;

static volatile unsigned sp = 0;
static volatile unsigned d = 1;

void sokoban::audio::streamer::fill_buffer(float *data, unsigned samples) {
  auto ssp = sp;
  float mult;
  if (ssp < 1000) {
    mult = ssp / 1000.0f;
  } else if (ssp < 4000) {
    mult = 1.0;
  } else if (ssp < 5000) {
    mult = (5000 - ssp) / 1000.0f;
  } else {
    mult = 0;
  }
  for (unsigned i = 0; i < samples; ++i) {
    data[i] = 0.25f * mult * ((ssp / d) % 2) - 0.5f;
    ssp++;
  }
  sp = ssp;
}

void sokoban::audio::streamer::play(unsigned div) {
  d = div;
  sp = 0;
}
