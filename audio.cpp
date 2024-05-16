module sokoban;
import siaudio;

static volatile unsigned sp = 0;
static volatile unsigned d = 1;
static void tune_filler(float *data, unsigned samples) {
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

static void null_filler(float *data, unsigned samples) {
  for (auto i = 0; i < samples; i++) {
    data[i] = 0;
  }
}

void setup_audio() {
  siaudio::filler(null_filler);
  siaudio::rate(44100);
}
void play_tone(unsigned div) {
  d = div;
  sp = 0;
  siaudio::filler(tune_filler);
}
