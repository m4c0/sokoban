export module save;
import buoy;
import silog;

export namespace save {
struct data {
  unsigned cur_level{};
};

data read() {
  return buoy::open_for_reading("sokoban", "save.dat")
      .fmap([](auto &&r) { return r->template read<data>(); })
      .unwrap(data{});
}

void write(data d) {
  buoy::open_for_writing("sokoban", "save.dat")
      .fmap([d](auto &&w) { return w->write(d); })
      .take([](auto err) {
        silog::log(silog::error, "failed to write save data: %s", err);
      });
}
} // namespace save
