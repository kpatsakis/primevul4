void CLASS parse_cine()
{
  unsigned off_head, off_setup, off_image, i;

  order = 0x4949;
  fseek (ifp, 4, SEEK_SET);
  is_raw = get2() == 2;
  fseek (ifp, 14, SEEK_CUR);
  is_raw *= get4();
  off_head = get4();
  off_setup = get4();
  off_image = get4();
  timestamp = get4();
  if ((i = get4())) timestamp = i;
  fseek (ifp, off_head+4, SEEK_SET);
  raw_width = get4();
  raw_height = get4();
  switch (get2(),get2()) {
    case  8:  load_raw = &CLASS eight_bit_load_raw;  break;
    case 16:  load_raw = &CLASS  unpacked_load_raw;
  }
  fseek (ifp, off_setup+792, SEEK_SET);
  strcpy (make, "CINE");
  sprintf (model, "%d", get4());
  fseek (ifp, 12, SEEK_CUR);
  switch ((i=get4()) & 0xffffff) {
    case  3:  filters = 0x94949494;  break;
    case  4:  filters = 0x49494949;  break;
    default:  is_raw = 0;
  }
  fseek (ifp, 72, SEEK_CUR);
  switch ((get4()+3600) % 360) {
    case 270:  flip = 4;  break;
    case 180:  flip = 1;  break;
    case  90:  flip = 7;  break;
    case   0:  flip = 2;
  }
  cam_mul[0] = getreal(11);
  cam_mul[2] = getreal(11);
  maximum = ~(-1 << get4());
  fseek (ifp, 668, SEEK_CUR);
  shutter = get4()/1000000000.0;
  fseek (ifp, off_image, SEEK_SET);
  if (shot_select < is_raw)
    fseek (ifp, shot_select*8, SEEK_CUR);
  data_offset  = (INT64) get4() + 8;
  data_offset += (INT64) get4() << 32;
}
