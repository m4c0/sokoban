export module save;
import buoy;
import silog;

export namespace save {
struct data {
  unsigned cur_level{};
  unsigned max_level{};
};

data read() {
  return buoy::open_for_reading("sokoban", "save.dat")
      .fmap([](auto &&r) { return r->template read<data>(); })
      .trace("reading save data")
      .log_error();
}

void write(data d) {
  buoy::open_for_writing("sokoban", "save.dat")
      .fmap([d](auto &&w) { return w->write(d); })
      .trace("writing save data")
      .log_error();
}
} // namespace save
