void CLASS parse_fuji (int offset)
{
  unsigned entries, tag, len, save, c;

  fseek (ifp, offset, SEEK_SET);
  entries = get4();
  if (entries > 255) return;
  while (entries--) {
    tag = get2();
    len = get2();
    save = ftell(ifp);
    if (tag == 0x100) {
      raw_height = get2();
      raw_width  = get2();
    } else if (tag == 0x121) {
      height = get2();
      if ((width = get2()) == 4284) width += 3;
    } else if (tag == 0x130) {
      fuji_layout = fgetc(ifp) >> 7;
      load_raw = fgetc(ifp) & 8 ?
	&CLASS unpacked_load_raw : &CLASS fuji_load_raw;
    } else if (tag == 0x2ff0) {
      FORC4 cam_mul[c ^ 1] = get2();
    } else if (tag == 0xc000) {
      c = order;
      order = 0x4949;
      width  = get4();
      height = get4();
      order = c;
    }
    fseek (ifp, save+len, SEEK_SET);
  }
  height <<= fuji_layout;
  width  >>= fuji_layout;
}
