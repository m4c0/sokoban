export module save;
import buoy;
import silog;

namespace save {
export unsigned read() {
  return buoy::open_for_reading("sokoban", "save.dat")
      .fmap([](auto &&r) { return r->read_u32(); })
      .unwrap(0);
}

export void write(unsigned idx) {
  buoy::open_for_writing("sokoban", "save.dat")
      .fmap([idx](auto &&w) { return w->write_u32(idx); })
      .take([](auto err) {
        silog::log(silog::error, "failed to write save data: %s", err);
      });
}
} // namespace save
