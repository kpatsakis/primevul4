void CLASS parse_redcine()
{
  int len;
  unsigned i, rdvo;

  order = 0x4d4d;
  is_raw = 0;
  fseek (ifp, 52, SEEK_SET);
  width  = get4();
  height = get4();
  fseek (ifp, 0, SEEK_END);
  fseek (ifp, -(i = ftello(ifp) & 511), SEEK_CUR);
  if (get4() != i || get4() != 0x52454f42) {
    dcraw_message (DCRAW_WARNING,
	    _("%s: Tail is missing, parsing from head...\n"), ifname_display);
    fseek (ifp, 0, SEEK_SET);
    while ((len = get4()) != EOF) {
      if (get4() == 0x52454456)
	if (is_raw++ == shot_select)
	  data_offset = ftello(ifp) - 8;
      fseek (ifp, len-8, SEEK_CUR);
    }
  } else {
    rdvo = get4();
    fseek (ifp, 12, SEEK_CUR);
    is_raw = get4();
    fseeko (ifp, rdvo+8 + shot_select*4, SEEK_SET);
    data_offset = get4();
  }
}
