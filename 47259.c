void CLASS identify()
{
  char head[32], *cp;
  int hlen, flen, fsize, zero_fsize=1, i, c, is_canon;
  struct jhead jh;
  short pana[][6] = {
    { 3130, 1743,  4,  0, -6,  0 },
    { 3130, 2055,  4,  0, -6,  0 },
    { 3130, 2319,  4,  0, -6,  0 },
    { 3170, 2103, 18,  0,-42, 20 },
    { 3170, 2367, 18, 13,-42,-21 },
    { 3177, 2367,  0,  0, -1,  0 },
    { 3304, 2458,  0,  0, -1,  0 },
    { 3330, 2463,  9,  0, -5,  0 },
    { 3330, 2479,  9,  0,-17,  4 },
    { 3370, 1899, 15,  0,-44, 20 },
    { 3370, 2235, 15,  0,-44, 20 },
    { 3370, 2511, 15, 10,-44,-21 },
    { 3690, 2751,  3,  0, -8, -3 },
    { 3710, 2751,  0,  0, -3,  0 },
    { 3724, 2450,  0,  0,  0, -2 },
    { 3770, 2487, 17,  0,-44, 19 },
    { 3770, 2799, 17, 15,-44,-19 },
    { 3880, 2170,  6,  0, -6,  0 },
    { 4060, 3018,  0,  0,  0, -2 },
    { 4290, 2391,  3,  0, -8, -1 },
    { 4330, 2439, 17, 15,-44,-19 },
    { 4508, 2962,  0,  0, -3, -4 },
    { 4508, 3330,  0,  0, -3, -6 } };
  static const struct {
    int fsize;
    const char make[12], model[19], withjpeg;
  } table[] = {
    {    62464, "Kodak",    "DC20"            ,0 },
    {   124928, "Kodak",    "DC20"            ,0 },
    {  1652736, "Kodak",    "DCS200"          ,0 },
    {  4159302, "Kodak",    "C330"            ,0 },
    {  4162462, "Kodak",    "C330"            ,0 },
    {   460800, "Kodak",    "C603v"           ,0 },
    {   614400, "Kodak",    "C603v"           ,0 },
    {  6163328, "Kodak",    "C603"            ,0 },
    {  6166488, "Kodak",    "C603"            ,0 },
    {  9116448, "Kodak",    "C603y"           ,0 },
    {   311696, "ST Micro", "STV680 VGA"      ,0 },  /* SPYz */
    {   787456, "Creative", "PC-CAM 600"      ,0 },
    {  1138688, "Minolta",  "RD175"           ,0 },
    {  3840000, "Foculus",  "531C"            ,0 },
    {   786432, "AVT",      "F-080C"          ,0 },
    {  1447680, "AVT",      "F-145C"          ,0 },
    {  1920000, "AVT",      "F-201C"          ,0 },
    {  5067304, "AVT",      "F-510C"          ,0 },
    {  5067316, "AVT",      "F-510C"          ,0 },
    { 10134608, "AVT",      "F-510C"          ,0 },
    { 10134620, "AVT",      "F-510C"          ,0 },
    { 16157136, "AVT",      "F-810C"          ,0 },
    {  1409024, "Sony",     "XCD-SX910CR"     ,0 },
    {  2818048, "Sony",     "XCD-SX910CR"     ,0 },
    {  3884928, "Micron",   "2010"            ,0 },
    {  6624000, "Pixelink", "A782"            ,0 },
    { 13248000, "Pixelink", "A782"            ,0 },
    {  6291456, "RoverShot","3320AF"          ,0 },
    {  6553440, "Canon",    "PowerShot A460"  ,0 },
    {  6653280, "Canon",    "PowerShot A530"  ,0 },
    {  6573120, "Canon",    "PowerShot A610"  ,0 },
    {  9219600, "Canon",    "PowerShot A620"  ,0 },
    {  9243240, "Canon",    "PowerShot A470"  ,0 },
    { 10341600, "Canon",    "PowerShot A720 IS",0 },
    { 10383120, "Canon",    "PowerShot A630"  ,0 },
    { 12945240, "Canon",    "PowerShot A640"  ,0 },
    { 15636240, "Canon",    "PowerShot A650"  ,0 },
    {  5298000, "Canon",    "PowerShot SD300" ,0 },
    {  7710960, "Canon",    "PowerShot S3 IS" ,0 },
    { 15467760, "Canon",    "PowerShot SX110 IS",0 },
    { 15534576, "Canon",    "PowerShot SX120 IS",0 },
    { 18653760, "Canon",    "PowerShot SX20 IS",0 },
    { 21936096, "Canon",    "PowerShot SX30 IS",0 },
    {  5939200, "OLYMPUS",  "C770UZ"          ,0 },
    {  1581060, "NIKON",    "E900"            ,1 },  /* or E900s,E910 */
    {  2465792, "NIKON",    "E950"            ,1 },  /* or E800,E700 */
    {  2940928, "NIKON",    "E2100"           ,1 },  /* or E2500 */
    {  4771840, "NIKON",    "E990"            ,1 },  /* or E995, Oly C3030Z */
    {  4775936, "NIKON",    "E3700"           ,1 },  /* or Optio 33WR */
    {  5869568, "NIKON",    "E4300"           ,1 },  /* or DiMAGE Z2 */
    {  5865472, "NIKON",    "E4500"           ,1 },
    {  7438336, "NIKON",    "E5000"           ,1 },  /* or E5700 */
    {  8998912, "NIKON",    "COOLPIX S6"      ,1 },
    {  1976352, "CASIO",    "QV-2000UX"       ,1 },
    {  3217760, "CASIO",    "QV-3*00EX"       ,1 },
    {  6218368, "CASIO",    "QV-5700"         ,1 },
    {  6054400, "CASIO",    "QV-R41"          ,1 },
    {  7530816, "CASIO",    "QV-R51"          ,1 },
    {  7684000, "CASIO",    "QV-4000"         ,1 },
    {  2937856, "CASIO",    "EX-S20"          ,1 },
    {  4948608, "CASIO",    "EX-S100"         ,1 },
    {  7542528, "CASIO",    "EX-Z50"          ,1 },
    {  7562048, "CASIO",    "EX-Z500"         ,1 },
    {  7753344, "CASIO",    "EX-Z55"          ,1 },
    {  7816704, "CASIO",    "EX-Z60"          ,1 },
    { 10843712, "CASIO",    "EX-Z75"          ,1 },
    { 10834368, "CASIO",    "EX-Z750"         ,1 },
    { 12310144, "CASIO",    "EX-Z850"         ,1 },
    { 15499264, "CASIO",    "EX-Z1050"        ,1 },
    {  7426656, "CASIO",    "EX-P505"         ,1 },
    {  9313536, "CASIO",    "EX-P600"         ,1 },
    { 10979200, "CASIO",    "EX-P700"         ,1 },
    {  3178560, "PENTAX",   "Optio S"         ,1 },
    {  4841984, "PENTAX",   "Optio S"         ,1 },
    {  6114240, "PENTAX",   "Optio S4"        ,1 },  /* or S4i, CASIO EX-Z4 */
    { 10702848, "PENTAX",   "Optio 750Z"      ,1 },
    { 15980544, "AGFAPHOTO","DC-833m"         ,1 },
    { 16098048, "SAMSUNG",  "S85"             ,1 },
    { 16215552, "SAMSUNG",  "S85"             ,1 },
    { 20487168, "SAMSUNG",  "WB550"           ,1 },
    { 24000000, "SAMSUNG",  "WB550"           ,1 },
    { 12582980, "Sinar",    ""                ,0 },
    { 33292868, "Sinar",    ""                ,0 },
    { 44390468, "Sinar",    ""                ,0 } };
  static const char *corp[] =
    { "Canon", "NIKON", "EPSON", "KODAK", "Kodak", "OLYMPUS", "PENTAX",
      "MINOLTA", "Minolta", "Konica", "CASIO", "Sinar", "Phase One",
      "SAMSUNG", "Mamiya", "MOTOROLA" };

  tiff_flip = flip = filters = -1;	/* 0 is valid, so -1 is unknown */
  raw_height = raw_width = fuji_width = fuji_layout = cr2_slice[0] = 0;
  maximum = height = width = top_margin = left_margin = 0;
  cdesc[0] = desc[0] = artist[0] = make[0] = model[0] = model2[0] = 0;
  iso_speed = shutter = aperture = focal_len = unique_id = 0;
  tiff_nifds = 0;
  memset (tiff_ifd, 0, sizeof tiff_ifd);
  memset (gpsdata, 0, sizeof gpsdata);
  memset (cblack, 0, sizeof cblack);
  memset (white, 0, sizeof white);
  thumb_offset = thumb_length = thumb_width = thumb_height = 0;
  load_raw = thumb_load_raw = 0;
  write_thumb = &CLASS jpeg_thumb;
  data_offset = meta_length = tiff_bps = tiff_compress = 0;
  kodak_cbpp = zero_after_ff = dng_version = load_flags = 0;
  timestamp = shot_order = tiff_samples = black = is_foveon = 0;
  mix_green = profile_length = data_error = zero_is_bad = 0;
  pixel_aspect = is_raw = raw_color = 1;
  tile_width = tile_length = INT_MAX;
  for (i=0; i < 4; i++) {
    cam_mul[i] = i == 1;
    pre_mul[i] = i < 3;
    FORC3 cmatrix[c][i] = 0;
    FORC3 rgb_cam[c][i] = c == i;
  }
  colors = 3;
  for (i=0; i < 0x4000; i++) curve[i] = i;

  order = get2();
  hlen = get4();
  fseek (ifp, 0, SEEK_SET);
  fread (head, 1, 32, ifp);
  fseek (ifp, 0, SEEK_END);
  flen = fsize = ftell(ifp);
  /* Note for Rawstudio maintainers, this check is not present in upstream dcraw */
  if (fsize < 32)
    return;
  if ((cp = (char *) memmem (head, 32, "MMMM", 4)) ||
      (cp = (char *) memmem (head, 32, "IIII", 4))) {
    parse_phase_one (cp-head);
    if (cp-head && parse_tiff(0)) apply_tiff();
  } else if (order == 0x4949 || order == 0x4d4d) {
    if (!memcmp (head+6,"HEAPCCDR",8)) {
      data_offset = hlen;
      parse_ciff (hlen, flen - hlen);
    } else if (parse_tiff(0)) apply_tiff();
  } else if (!memcmp (head,"\xff\xd8\xff\xe1",4) &&
	     !memcmp (head+6,"Exif",4)) {
    fseek (ifp, 4, SEEK_SET);
    data_offset = 4 + get2();
    fseek (ifp, data_offset, SEEK_SET);
    if (fgetc(ifp) != 0xff)
      parse_tiff(12);
    thumb_offset = 0;
  } else if (!memcmp (head+25,"ARECOYK",7)) {
    strcpy (make, "Contax");
    strcpy (model,"N Digital");
    fseek (ifp, 33, SEEK_SET);
    get_timestamp(1);
    fseek (ifp, 60, SEEK_SET);
    FORC4 cam_mul[c ^ (c >> 1)] = get4();
  } else if (!strcmp (head, "PXN")) {
    strcpy (make, "Logitech");
    strcpy (model,"Fotoman Pixtura");
  } else if (!strcmp (head, "qktk")) {
    strcpy (make, "Apple");
    strcpy (model,"QuickTake 100");
    load_raw = &CLASS quicktake_100_load_raw;
  } else if (!strcmp (head, "qktn")) {
    strcpy (make, "Apple");
    strcpy (model,"QuickTake 150");
    load_raw = &CLASS kodak_radc_load_raw;
  } else if (!memcmp (head,"FUJIFILM",8)) {
    fseek (ifp, 84, SEEK_SET);
    thumb_offset = get4();
    thumb_length = get4();
    fseek (ifp, 92, SEEK_SET);
    parse_fuji (get4());
    if (thumb_offset > 120) {
      fseek (ifp, 120, SEEK_SET);
      is_raw += (i = get4()) && 1;
      if (is_raw == 2 && shot_select)
	parse_fuji (i);
    }
    fseek (ifp, 100+28*(shot_select > 0), SEEK_SET);
    parse_tiff (data_offset = get4());
    parse_tiff (thumb_offset+12);
    apply_tiff();
  } else if (!memcmp (head,"RIFF",4)) {
    fseek (ifp, 0, SEEK_SET);
    parse_riff();
  } else if (!memcmp (head,"\0\001\0\001\0@",6)) {
    fseek (ifp, 6, SEEK_SET);
    fread (make, 1, 8, ifp);
    fread (model, 1, 8, ifp);
    fread (model2, 1, 16, ifp);
    data_offset = get2();
    get2();
    raw_width = get2();
    raw_height = get2();
    load_raw = &CLASS nokia_load_raw;
    filters = 0x61616161;
  } else if (!memcmp (head,"NOKIARAW",8)) {
    strcpy (make, "NOKIA");
    strcpy (model, "X2");
    order = 0x4949;
    fseek (ifp, 300, SEEK_SET);
    data_offset = get4();
    i = get4();
    width = get2();
    height = get2();
    data_offset += i - width * 5 / 4 * height;
    load_raw = &CLASS nokia_load_raw;
    filters = 0x61616161;
  } else if (!memcmp (head,"ARRI",4)) {
    order = 0x4949;
    fseek (ifp, 20, SEEK_SET);
    width = get4();
    height = get4();
    strcpy (make, "ARRI");
    fseek (ifp, 668, SEEK_SET);
    fread (model, 1, 64, ifp);
    data_offset = 4096;
    load_raw = &CLASS packed_load_raw;
    load_flags = 88;
    filters = 0x61616161;
  } else if (!memcmp (head+4,"RED1",4)) {
    strcpy (make, "RED");
    strcpy (model,"ONE");
    parse_redcine();
    load_raw = &CLASS redcine_load_raw;
    gamma_curve (1/2.4, 12.92, 1, 4095);
    filters = 0x49494949;
  } else if (!memcmp (head,"DSC-Image",9))
    parse_rollei();
  else if (!memcmp (head,"PWAD",4))
    parse_sinar_ia();
  else if (!memcmp (head,"\0MRM",4))
    parse_minolta(0);
  else if (!memcmp (head,"FOVb",4))
    parse_foveon();
  else if (!memcmp (head,"CI",2))
    parse_cine();
  else
    for (zero_fsize=i=0; i < (int) sizeof table / (int) sizeof *table; i++)
      if (fsize == table[i].fsize) {
	strcpy (make,  table[i].make );
	strcpy (model, table[i].model);
	if (table[i].withjpeg)
	  parse_external_jpeg();
      }
  if (zero_fsize) fsize = 0;
  if (make[0] == 0) parse_smal (0, flen);
  if (make[0] == 0) parse_jpeg (is_raw = 0);

  for (i=0; i < (int) sizeof corp / (int) sizeof *corp; i++)
    if (strstr (make, corp[i]))		/* Simplify company names */
	strcpy (make, corp[i]);
  if (!strncmp (make,"KODAK",5) &&
	((cp = strstr(model," DIGITAL CAMERA")) ||
	 (cp = strstr(model," Digital Camera")) ||
	 (cp = strstr(model,"FILE VERSION"))))
     *cp = 0;
  cp = make + strlen(make);		/* Remove trailing spaces */
  while (*--cp == ' ') *cp = 0;
  cp = model + strlen(model);
  while (*--cp == ' ') *cp = 0;
  i = strlen(make);			/* Remove make from model */
  if (!strncasecmp (model, make, i) && model[i++] == ' ')
    memmove (model, model+i, 64-i);
  if (!strncmp (model,"Digital Camera ",15))
    strcpy (model, model+15);
  desc[511] = artist[63] = make[63] = model[63] = model2[63] = 0;
  if (!is_raw) goto notraw;

  if (!height) height = raw_height;
  if (!width)  width  = raw_width;
  if (fuji_width) {
    fuji_width = (raw_width+1)/2;
    width = height + fuji_width;
    height = width - 1;
    pixel_aspect = 1;
  }
  if (height == 2624 && width == 3936)	/* Pentax K10D and Samsung GX10 */
    { height  = 2616;   width  = 3896; }
  if (height == 3136 && width == 4864)  /* Pentax K20D and Samsung GX20 */
    { height  = 3124;   width  = 4688; filters = 0x16161616; }
  if (width == 4352 && (!strcmp(model,"K-r") || !strcmp(model,"K-x")))
    {			width  = 4309; filters = 0x16161616; }
  if (width >= 4960 && !strcmp(model,"K-5"))
    { left_margin = 10; width  = 4950; filters = 0x16161616; }
  if (width == 4736 && !strcmp(model,"K-7"))
    { height  = 3122;   width  = 4684; filters = 0x16161616; top_margin = 2; }
  if (width == 7424 && !strcmp(model,"645D"))
    { height  = 5502;   width  = 7328; filters = 0x61616161; top_margin = 29;
      left_margin = 48; }
  if (height == 3014 && width == 4096)	/* Ricoh GX200 */
			width  = 4014;
  if (dng_version) {
    if (filters == UINT_MAX) filters = 0;
    if (filters) is_raw = tiff_samples;
    else	 colors = tiff_samples;
    if (tiff_compress == 1)
      load_raw = &CLASS adobe_dng_load_raw_nc;
    if (tiff_compress == 7)
      load_raw = &CLASS adobe_dng_load_raw_lj;
    goto dng_skip;
  }
  if ((is_canon = !strcmp(make,"Canon")))
    load_raw = memcmp (head+6,"HEAPCCDR",8) ?
	&CLASS lossless_jpeg_load_raw : &CLASS canon_compressed_load_raw;
  if (!strcmp(make,"NIKON")) {
    if (!load_raw)
      load_raw = &CLASS packed_load_raw;
    if (model[0] == 'E')
      load_flags |= !data_offset << 2 | 2;
  }
  if (!strcmp(make,"CASIO")) {
    load_raw = &CLASS packed_load_raw;
    maximum = 0xf7f;
  }

/* Set parameters based on camera name (for non-DNG files). */

  if (is_foveon) {
    if (height*2 < width) pixel_aspect = 0.5;
    if (height   > width) pixel_aspect = 2;
    filters = 0;
    load_raw = &CLASS foveon_load_raw;
    simple_coeff(0);
  } else if (is_canon && tiff_bps == 15) {
    switch (width) {
      case 3344: width -= 66;
      case 3872: width -= 6;
    }
    filters = 0;
    load_raw = &CLASS canon_sraw_load_raw;
  } else if (!strcmp(model,"PowerShot 600")) {
    height = 613;
    width  = 854;
    raw_width = 896;
    pixel_aspect = 607/628.0;
    colors = 4;
    filters = 0xe1e4e1e4;
    load_raw = &CLASS canon_600_load_raw;
  } else if (!strcmp(model,"PowerShot A5") ||
	     !strcmp(model,"PowerShot A5 Zoom")) {
    height = 773;
    width  = 960;
    raw_width = 992;
    pixel_aspect = 256/235.0;
    colors = 4;
    filters = 0x1e4e1e4e;
    goto canon_a5;
  } else if (!strcmp(model,"PowerShot A50")) {
    height =  968;
    width  = 1290;
    raw_width = 1320;
    colors = 4;
    filters = 0x1b4e4b1e;
    goto canon_a5;
  } else if (!strcmp(model,"PowerShot Pro70")) {
    height = 1024;
    width  = 1552;
    colors = 4;
    filters = 0x1e4b4e1b;
    goto canon_a5;
  } else if (!strcmp(model,"PowerShot SD300")) {
    height = 1752;
    width  = 2344;
    raw_height = 1766;
    raw_width  = 2400;
    top_margin  = 12;
    left_margin = 12;
    goto canon_a5;
  } else if (!strcmp(model,"PowerShot A460")) {
    height = 1960;
    width  = 2616;
    raw_height = 1968;
    raw_width  = 2664;
    top_margin  = 4;
    left_margin = 4;
    goto canon_a5;
  } else if (!strcmp(model,"PowerShot A530")) {
    height = 1984;
    width  = 2620;
    raw_height = 1992;
    raw_width  = 2672;
    top_margin  = 6;
    left_margin = 10;
    goto canon_a5;
  } else if (!strcmp(model,"PowerShot A610")) {
    if (canon_s2is()) strcpy (model+10, "S2 IS");
    height = 1960;
    width  = 2616;
    raw_height = 1968;
    raw_width  = 2672;
    top_margin  = 8;
    left_margin = 12;
    goto canon_a5;
  } else if (!strcmp(model,"PowerShot A620")) {
    height = 2328;
    width  = 3112;
    raw_height = 2340;
    raw_width  = 3152;
    top_margin  = 12;
    left_margin = 36;
    goto canon_a5;
  } else if (!strcmp(model,"PowerShot A470")) {
    height = 2328;
    width  = 3096;
    raw_height = 2346;
    raw_width  = 3152;
    top_margin  = 6;
    left_margin = 12;
    goto canon_a5;
  } else if (!strcmp(model,"PowerShot A720 IS")) {
    height = 2472;
    width  = 3298;
    raw_height = 2480;
    raw_width  = 3336;
    top_margin  = 5;
    left_margin = 6;
    goto canon_a5;
  } else if (!strcmp(model,"PowerShot A630")) {
    height = 2472;
    width  = 3288;
    raw_height = 2484;
    raw_width  = 3344;
    top_margin  = 6;
    left_margin = 12;
    goto canon_a5;
  } else if (!strcmp(model,"PowerShot A640")) {
    height = 2760;
    width  = 3672;
    raw_height = 2772;
    raw_width  = 3736;
    top_margin  = 6;
    left_margin = 12;
    goto canon_a5;
  } else if (!strcmp(model,"PowerShot A650")) {
    height = 3024;
    width  = 4032;
    raw_height = 3048;
    raw_width  = 4104;
    top_margin  = 12;
    left_margin = 48;
    goto canon_a5;
  } else if (!strcmp(model,"PowerShot S3 IS")) {
    height = 2128;
    width  = 2840;
    raw_height = 2136;
    raw_width  = 2888;
    top_margin  = 8;
    left_margin = 44;
canon_a5:
    tiff_bps = 10;
    load_raw = &CLASS packed_load_raw;
    load_flags = 40;
    if (raw_width > 1600) zero_is_bad = 1;
  } else if (!strcmp(model,"PowerShot SX110 IS")) {
    height = 2760;
    width  = 3684;
    raw_height = 2772;
    raw_width  = 3720;
    top_margin  = 12;
    left_margin = 6;
    load_raw = &CLASS packed_load_raw;
    load_flags = 40;
    zero_is_bad = 1;
  } else if (!strcmp(model,"PowerShot SX120 IS")) {
    height = 2742;
    width  = 3664;
    raw_height = 2778;
    raw_width  = 3728;
    top_margin  = 18;
    left_margin = 16;
    filters = 0x49494949;
    load_raw = &CLASS packed_load_raw;
    load_flags = 40;
    zero_is_bad = 1;
  } else if (!strcmp(model,"PowerShot SX20 IS")) {
    height = 3024;
    width  = 4032;
    raw_height = 3048;
    raw_width  = 4080;
    top_margin  = 12;
    left_margin = 24;
    load_raw = &CLASS packed_load_raw;
    load_flags = 40;
    zero_is_bad = 1;
  } else if (!strcmp(model,"PowerShot SX30 IS")) {
    height = 3254;
    width  = 4366;
    raw_height = 3276;
    raw_width  = 4464;
    top_margin  = 10;
    left_margin = 25;
    filters = 0x16161616;
    load_raw = &CLASS packed_load_raw;
    load_flags = 40;
    zero_is_bad = 1;
  } else if (!strcmp(model,"PowerShot Pro90 IS")) {
    width  = 1896;
    colors = 4;
    filters = 0xb4b4b4b4;
  } else if (is_canon && raw_width == 2144) {
    height = 1550;
    width  = 2088;
    top_margin  = 8;
    left_margin = 4;
    if (!strcmp(model,"PowerShot G1")) {
      colors = 4;
      filters = 0xb4b4b4b4;
    }
  } else if (is_canon && raw_width == 2224) {
    height = 1448;
    width  = 2176;
    top_margin  = 6;
    left_margin = 48;
  } else if (is_canon && raw_width == 2376) {
    height = 1720;
    width  = 2312;
    top_margin  = 6;
    left_margin = 12;
  } else if (is_canon && raw_width == 2672) {
    height = 1960;
    width  = 2616;
    top_margin  = 6;
    left_margin = 12;
  } else if (is_canon && raw_width == 3152) {
    height = 2056;
    width  = 3088;
    top_margin  = 12;
    left_margin = 64;
    if (unique_id == 0x80000170)
      adobe_coeff ("Canon","EOS 300D");
  } else if (is_canon && raw_width == 3160) {
    height = 2328;
    width  = 3112;
    top_margin  = 12;
    left_margin = 44;
  } else if (is_canon && raw_width == 3344) {
    height = 2472;
    width  = 3288;
    top_margin  = 6;
    left_margin = 4;
  } else if (!strcmp(model,"EOS D2000C")) {
    filters = 0x61616161;
    black = curve[200];
  } else if (is_canon && raw_width == 3516) {
    top_margin  = 14;
    left_margin = 42;
    if (unique_id == 0x80000189)
      adobe_coeff ("Canon","EOS 350D");
    goto canon_cr2;
  } else if (is_canon && raw_width == 3596) {
    top_margin  = 12;
    left_margin = 74;
    goto canon_cr2;
  } else if (is_canon && raw_width == 3744) {
    height = 2760;
    width  = 3684;
    top_margin  = 16;
    left_margin = 8;
    if (unique_id > 0x2720000) {
      top_margin  = 12;
      left_margin = 52;
    }
  } else if (is_canon && raw_width == 3944) {
    height = 2602;
    width  = 3908;
    top_margin  = 18;
    left_margin = 30;
  } else if (is_canon && raw_width == 3948) {
    top_margin  = 18;
    left_margin = 42;
    height -= 2;
    if (unique_id == 0x80000236)
      adobe_coeff ("Canon","EOS 400D");
    if (unique_id == 0x80000254)
      adobe_coeff ("Canon","EOS 1000D");
    goto canon_cr2;
  } else if (is_canon && raw_width == 3984) {
    top_margin  = 20;
    left_margin = 76;
    height -= 2;
    goto canon_cr2;
  } else if (is_canon && raw_width == 4104) {
    height = 3024;
    width  = 4032;
    top_margin  = 12;
    left_margin = 48;
  } else if (is_canon && raw_width == 4152) {
    top_margin  = 12;
    left_margin = 192;
    goto canon_cr2;
  } else if (is_canon && raw_width == 4160) {
    height = 3048;
    width  = 4048;
    top_margin  = 11;
    left_margin = 104;
  } else if (is_canon && raw_width == 4312) {
    top_margin  = 18;
    left_margin = 22;
    height -= 2;
    if (unique_id == 0x80000176)
      adobe_coeff ("Canon","EOS 450D");
    goto canon_cr2;
  } else if (is_canon && raw_width == 4352) {
    top_margin  = 18;
    left_margin = 62;
    if (unique_id == 0x80000288)
      adobe_coeff ("Canon","EOS 1100D");
    goto canon_cr2;
  } else if (is_canon && raw_width == 4476) {
    top_margin  = 34;
    left_margin = 90;
    goto canon_cr2;
  } else if (is_canon && raw_width == 4480) {
    height = 3326;
    width  = 4432;
    top_margin  = 10;
    left_margin = 12;
    filters = 0x49494949;
  } else if (is_canon && raw_width == 4832) {
    top_margin = unique_id == 0x80000261 ? 51:26;
    left_margin = 62;
    if (unique_id == 0x80000252)
      adobe_coeff ("Canon","EOS 500D");
    goto canon_cr2;
  } else if (is_canon && raw_width == 5120) {
    height -= top_margin = 45;
    left_margin = 142;
    width = 4916;
  } else if (is_canon && raw_width == 5344) {
    top_margin = 51;
    left_margin = 142;
    if (unique_id == 0x80000270)
      adobe_coeff ("Canon","EOS 550D");
    if (unique_id == 0x80000286)
      adobe_coeff ("Canon","EOS 600D");
    goto canon_cr2;
  } else if (is_canon && raw_width == 5360) {
    top_margin = 51;
    left_margin = 158;
    goto canon_cr2;
  } else if (is_canon && raw_width == 5792) {
    top_margin  = 51;
    left_margin = 158;
    goto canon_cr2;
  } else if (is_canon && raw_width == 5108) {
    top_margin  = 13;
    left_margin = 98;
canon_cr2:
    height -= top_margin;
    width  -= left_margin;
  } else if (is_canon && raw_width == 5712) {
    height = 3752;
    width  = 5640;
    top_margin  = 20;
    left_margin = 62;
  } else if (!strcmp(model,"D1")) {
    cam_mul[0] *= 256/527.0;
    cam_mul[2] *= 256/317.0;
  } else if (!strcmp(model,"D1X")) {
    width -= 4;
    pixel_aspect = 0.5;
  } else if (!strcmp(model,"D40X") ||
	     !strcmp(model,"D60")  ||
	     !strcmp(model,"D80")  ||
	     !strcmp(model,"D3000")) {
    height -= 3;
    width  -= 4;
  } else if (!strcmp(model,"D3")   ||
	     !strcmp(model,"D3S")  ||
	     !strcmp(model,"D700")) {
    width -= 4;
    left_margin = 2;
  } else if (!strcmp(model,"D5000")) {
    width -= 42;
  } else if (!strcmp(model,"D5100") ||
	     !strcmp(model,"D7000")) {
    width -= 44;
  } else if (!strcmp(model,"D3100")) {
    width -= 28;
    left_margin = 6;
  } else if (!strncmp(model,"D40",3) ||
	     !strncmp(model,"D50",3) ||
	     !strncmp(model,"D70",3)) {
    width--;
  } else if (!strcmp(model,"D90")) {
    width -= 42;
  } else if (!strcmp(model,"D100")) {
    if (tiff_compress == 34713 && !nikon_is_compressed()) {
      load_raw = &CLASS packed_load_raw;
      load_flags |= 1;
      raw_width = (width += 3) + 3;
    }
  } else if (!strcmp(model,"D200")) {
    left_margin = 1;
    width -= 4;
    filters = 0x94949494;
  } else if (!strncmp(model,"D2H",3)) {
    left_margin = 6;
    width -= 14;
  } else if (!strncmp(model,"D2X",3)) {
    if (width == 3264) width -= 32;
    else width -= 8;
  } else if (!strncmp(model,"D300",4)) {
    width -= 32;
  } else if (!strncmp(model,"COOLPIX P",9)) {
    load_flags = 24;
    filters = 0x94949494;
    if (model[9] == '7' && iso_speed >= 400)
      black = 255;
  } else if (!strncmp(model,"1 ",2)) {
    height -= 2;
  } else if (fsize == 1581060) {
    height = 963;
    width = 1287;
    raw_width = 1632;
    maximum = 0x3f4;
    colors = 4;
    filters = 0x1e1e1e1e;
    simple_coeff(3);
    pre_mul[0] = 1.2085;
    pre_mul[1] = 1.0943;
    pre_mul[3] = 1.1103;
    goto e900;
  } else if (fsize == 2465792) {
    height = 1203;
    width  = 1616;
    raw_width = 2048;
    colors = 4;
    filters = 0x4b4b4b4b;
    adobe_coeff ("NIKON","E950");
e900:
    tiff_bps = 10;
    load_raw = &CLASS packed_load_raw;
    load_flags = 6;
  } else if (fsize == 4771840) {
    height = 1540;
    width  = 2064;
    colors = 4;
    filters = 0xe1e1e1e1;
    load_raw = &CLASS packed_load_raw;
    load_flags = 6;
    if (!timestamp && nikon_e995())
      strcpy (model, "E995");
    if (strcmp(model,"E995")) {
      filters = 0xb4b4b4b4;
      simple_coeff(3);
      pre_mul[0] = 1.196;
      pre_mul[1] = 1.246;
      pre_mul[2] = 1.018;
    }
  } else if (!strcmp(model,"E2100")) {
    if (!timestamp && !nikon_e2100()) goto cp_e2500;
    height = 1206;
    width  = 1616;
    load_flags = 30;
  } else if (!strcmp(model,"E2500")) {
cp_e2500:
    strcpy (model, "E2500");
    height = 1204;
    width  = 1616;
    colors = 4;
    filters = 0x4b4b4b4b;
  } else if (fsize == 4775936) {
    height = 1542;
    width  = 2064;
    load_raw = &CLASS packed_load_raw;
    load_flags = 30;
    if (!timestamp) nikon_3700();
    if (model[0] == 'E' && atoi(model+1) < 3700)
      filters = 0x49494949;
    if (!strcmp(model,"Optio 33WR")) {
      flip = 1;
      filters = 0x16161616;
    }
    if (make[0] == 'O') {
      i = find_green (12, 32, 1188864, 3576832);
      c = find_green (12, 32, 2383920, 2387016);
      if (abs(i) < abs(c)) {
	SWAP(i,c);
	load_flags = 24;
      }
      if (i < 0) filters = 0x61616161;
    }
  } else if (fsize == 5869568) {
    height = 1710;
    width  = 2288;
    filters = 0x16161616;
    if (!timestamp && minolta_z2()) {
      strcpy (make, "Minolta");
      strcpy (model,"DiMAGE Z2");
    }
    load_raw = &CLASS packed_load_raw;
    load_flags = 6 + 24*(make[0] == 'M');
  } else if (!strcmp(model,"E4500")) {
    height = 1708;
    width  = 2288;
    colors = 4;
    filters = 0xb4b4b4b4;
  } else if (fsize == 7438336) {
    height = 1924;
    width  = 2576;
    colors = 4;
    filters = 0xb4b4b4b4;
  } else if (fsize == 8998912) {
    height = 2118;
    width  = 2832;
    maximum = 0xf83;
    load_raw = &CLASS packed_load_raw;
    load_flags = 30;
  } else if (!strcmp(model,"FinePix S5100") ||
	     !strcmp(model,"FinePix S5500")) {
    height -= top_margin = 6;
  } else if (!strcmp(make,"FUJIFILM")) {
    if (!strcmp(model+7,"S2Pro")) {
      strcpy (model+7," S2Pro");
      height = 2144;
      width  = 2880;
      flip = 6;
    } else if (load_raw != &CLASS packed_load_raw)
      maximum = (is_raw == 2 && shot_select) ? 0x2f00 : 0x3e00;
    top_margin = (raw_height - height) >> 2 << 1;
    left_margin = (raw_width - width ) >> 2 << 1;
    if (width == 3328) {
      width = 3262;
      left_margin = 34;
    }
    if (!strcmp(model,"X10"))
      filters = 0x16161616;
    if (fuji_layout) raw_width *= is_raw;
    if (load_raw == &CLASS fuji_load_raw) {
      fuji_width = width >> !fuji_layout;
      width = (height >> fuji_layout) + fuji_width;
      raw_height = height;
      height = width - 1;
      if (~fuji_width & 1) filters = 0x49494949;
    }
  } else if (!strcmp(model,"RD175")) {
    height = 986;
    width = 1534;
    data_offset = 513;
    filters = 0x61616161;
    load_raw = &CLASS minolta_rd175_load_raw;
  } else if (!strcmp(model,"KD-400Z")) {
    height = 1712;
    width  = 2312;
    raw_width = 2336;
    goto konica_400z;
  } else if (!strcmp(model,"KD-510Z")) {
    goto konica_510z;
  } else if (!strcasecmp(make,"MINOLTA")) {
    load_raw = &CLASS unpacked_load_raw;
    maximum = 0xfff;
    if (!strncmp(model,"DiMAGE A",8)) {
      if (!strcmp(model,"DiMAGE A200"))
	filters = 0x49494949;
      tiff_bps = 12;
      load_raw = &CLASS packed_load_raw;
    } else if (!strncmp(model,"ALPHA",5) ||
	       !strncmp(model,"DYNAX",5) ||
	       !strncmp(model,"MAXXUM",6)) {
      sprintf (model+20, "DYNAX %-10s", model+6+(model[0]=='M'));
      adobe_coeff (make, model+20);
      load_raw = &CLASS packed_load_raw;
    } else if (!strncmp(model,"DiMAGE G",8)) {
      if (model[8] == '4') {
	height = 1716;
	width  = 2304;
      } else if (model[8] == '5') {
konica_510z:
	height = 1956;
	width  = 2607;
	raw_width = 2624;
      } else if (model[8] == '6') {
	height = 2136;
	width  = 2848;
      }
      data_offset += 14;
      filters = 0x61616161;
konica_400z:
      load_raw = &CLASS unpacked_load_raw;
      maximum = 0x3df;
      order = 0x4d4d;
    }
  } else if (!strcmp(model,"*ist D")) {
    data_error = -1;
  } else if (!strcmp(model,"*ist DS")) {
    height -= 2;
  } else if (!strcmp(model,"Optio S")) {
    if (fsize == 3178560) {
      height = 1540;
      width  = 2064;
      load_raw = &CLASS eight_bit_load_raw;
      cam_mul[0] *= 4;
      cam_mul[2] *= 4;
    } else {
      height = 1544;
      width  = 2068;
      raw_width = 3136;
      load_raw = &CLASS packed_load_raw;
      maximum = 0xf7c;
    }
  } else if (fsize == 6114240) {
    height = 1737;
    width  = 2324;
    raw_width = 3520;
    load_raw = &CLASS packed_load_raw;
    maximum = 0xf7a;
  } else if (!strcmp(model,"Optio 750Z")) {
    height = 2302;
    width  = 3072;
    load_raw = &CLASS packed_load_raw;
    load_flags = 30;
  } else if (!strcmp(model,"DC-833m")) {
    height = 2448;
    width  = 3264;
    order = 0x4949;
    filters = 0x61616161;
    load_raw = &CLASS unpacked_load_raw;
    maximum = 0xfc00;
  } else if (!strncmp(model,"S85",3)) {
    height = 2448;
    width  = 3264;
    raw_width = fsize/height/2;
    order = 0x4d4d;
    load_raw = &CLASS unpacked_load_raw;
  } else if (!strncmp(model,"NX1",3)) {
    height -= top_margin = 8;
    width -= 2 * (left_margin = 8);
    load_flags = 32;
  } else if (!strcmp(model,"NX200")) {
    order = 0x4949;
    height = 3662;
    width  = 5528;
    top_margin = 2;
    left_margin = 46;
  } else if (!strcmp(model,"EX1")) {
    order = 0x4949;
    height -= 20;
    top_margin = 2;
    if ((width -= 6) > 3682) {
      height -= 10;
      width  -= 46;
      top_margin = 8;
    }
  } else if (!strcmp(model,"WB2000")) {
    order = 0x4949;
    height -= 3;
    top_margin = 2;
    if ((width -= 10) > 3718) {
      height -= 28;
      width  -= 56;
      top_margin = 8;
    }
  } else if (fsize == 20487168) {
    height = 2808;
    width  = 3648;
    goto wb550;
  } else if (fsize == 24000000) {
    height = 3000;
    width  = 4000;
wb550:
    strcpy (model, "WB550");
    order = 0x4d4d;
    load_raw = &CLASS unpacked_load_raw;
    load_flags = 6;
    maximum = 0x3df;
  } else if (!strcmp(model,"STV680 VGA")) {
    height = 484;
    width  = 644;
    load_raw = &CLASS eight_bit_load_raw;
    flip = 2;
    filters = 0x16161616;
    black = 16;
  } else if (!strcmp(model,"N95")) {
    height = raw_height - (top_margin = 2);
  } else if (!strcmp(model,"531C")) {
    height = 1200;
    width  = 1600;
    load_raw = &CLASS unpacked_load_raw;
    filters = 0x49494949;
  } else if (!strcmp(model,"F-080C")) {
    height = 768;
    width  = 1024;
    load_raw = &CLASS eight_bit_load_raw;
  } else if (!strcmp(model,"F-145C")) {
    height = 1040;
    width  = 1392;
    load_raw = &CLASS eight_bit_load_raw;
  } else if (!strcmp(model,"F-201C")) {
    height = 1200;
    width  = 1600;
    load_raw = &CLASS eight_bit_load_raw;
  } else if (!strcmp(model,"F-510C")) {
    height = 1958;
    width  = 2588;
    load_raw = fsize < 7500000 ?
	&CLASS eight_bit_load_raw : &CLASS unpacked_load_raw;
    data_offset = fsize - width*height*(fsize >> 22);
    maximum = 0xfff0;
  } else if (!strcmp(model,"F-810C")) {
    height = 2469;
    width  = 3272;
    load_raw = &CLASS unpacked_load_raw;
    maximum = 0xfff0;
  } else if (!strcmp(model,"XCD-SX910CR")) {
    height = 1024;
    width  = 1375;
    raw_width = 1376;
    filters = 0x49494949;
    maximum = 0x3ff;
    load_raw = fsize < 2000000 ?
	&CLASS eight_bit_load_raw : &CLASS unpacked_load_raw;
  } else if (!strcmp(model,"2010")) {
    height = 1207;
    width  = 1608;
    order = 0x4949;
    filters = 0x16161616;
    data_offset = 3212;
    maximum = 0x3ff;
    load_raw = &CLASS unpacked_load_raw;
  } else if (!strcmp(model,"A782")) {
    height = 3000;
    width  = 2208;
    filters = 0x61616161;
    load_raw = fsize < 10000000 ?
	&CLASS eight_bit_load_raw : &CLASS unpacked_load_raw;
    maximum = 0xffc0;
  } else if (!strcmp(model,"3320AF")) {
    height = 1536;
    raw_width = width = 2048;
    filters = 0x61616161;
    load_raw = &CLASS unpacked_load_raw;
    maximum = 0x3ff;
    fseek (ifp, 0x300000, SEEK_SET);
    if ((order = guess_byte_order(0x10000)) == 0x4d4d) {
      height -= (top_margin = 16);
      width -= (left_margin = 28);
      maximum = 0xf5c0;
      strcpy (make, "ISG");
      model[0] = 0;
    }
  } else if (!strcmp(make,"Hasselblad")) {
    if (load_raw == &CLASS lossless_jpeg_load_raw)
      load_raw = &CLASS hasselblad_load_raw;
    if (raw_width == 7262) {
      height = 5444;
      width  = 7248;
      top_margin  = 4;
      left_margin = 7;
      filters = 0x61616161;
    } else if (raw_width == 7410) {
      height = 5502;
      width  = 7328;
      top_margin  = 4;
      left_margin = 41;
      filters = 0x61616161;
    } else if (raw_width == 9044) {
      height = 6716;
      width  = 8964;
      top_margin  = 8;
      left_margin = 40;
      black += load_flags = 256;
      maximum = 0x8101;
    } else if (raw_width == 4090) {
      strcpy (model, "V96C");
      height -= (top_margin = 6);
      width -= (left_margin = 3) + 7;
      filters = 0x61616161;
    }
  } else if (!strcmp(make,"Sinar")) {
    if (!memcmp(head,"8BPS",4)) {
      fseek (ifp, 14, SEEK_SET);
      height = get4();
      width  = get4();
      filters = 0x61616161;
      data_offset = 68;
    }
    if (!load_raw) load_raw = &CLASS unpacked_load_raw;
    maximum = 0x3fff;
  } else if (!strcmp(make,"Leaf")) {
    maximum = 0x3fff;
    fseek (ifp, data_offset, SEEK_SET);
    if (ljpeg_start (&jh, 1) && jh.bits == 15)
      maximum = 0x1fff;
    if (tiff_samples > 1) filters = 0;
    if (tiff_samples > 1 || tile_length < raw_height) {
      load_raw = &CLASS leaf_hdr_load_raw;
      raw_width = tile_width;
    }
    if ((width | height) == 2048) {
      if (tiff_samples == 1) {
	filters = 1;
	strcpy (cdesc, "RBTG");
	strcpy (model, "CatchLight");
	top_margin =  8; left_margin = 18; height = 2032; width = 2016;
      } else {
	strcpy (model, "DCB2");
	top_margin = 10; left_margin = 16; height = 2028; width = 2022;
      }
    } else if (width+height == 3144+2060) {
      if (!model[0]) strcpy (model, "Cantare");
      if (width > height) {
	 top_margin = 6; left_margin = 32; height = 2048;  width = 3072;
	filters = 0x61616161;
      } else {
	left_margin = 6;  top_margin = 32;  width = 2048; height = 3072;
	filters = 0x16161616;
      }
      if (!cam_mul[0] || model[0] == 'V') filters = 0;
      else is_raw = tiff_samples;
    } else if (width == 2116) {
      strcpy (model, "Valeo 6");
      height -= 2 * (top_margin = 30);
      width -= 2 * (left_margin = 55);
      filters = 0x49494949;
    } else if (width == 3171) {
      strcpy (model, "Valeo 6");
      height -= 2 * (top_margin = 24);
      width -= 2 * (left_margin = 24);
      filters = 0x16161616;
    }
  } else if (!strcmp(make,"LEICA") || !strcmp(make,"Panasonic")) {
    if ((flen - data_offset) / (raw_width*8/7) == raw_height)
      load_raw = &CLASS panasonic_load_raw;
    if (!load_raw) {
      load_raw = &CLASS unpacked_load_raw;
      load_flags = 4;
    }
    zero_is_bad = 1;
    if ((height += 12) > raw_height) height = raw_height;
    for (i=0; i < (int) sizeof pana / (int) sizeof *pana; i++)
      if (raw_width == pana[i][0] && raw_height == pana[i][1]) {
	left_margin = pana[i][2];
	 top_margin = pana[i][3];
	     width += pana[i][4];
	    height += pana[i][5];
      }
    filters = 0x01010101 * (uchar) "\x94\x61\x49\x16"
	[((filters-1) ^ (left_margin & 1) ^ (top_margin << 1)) & 3];
  } else if (!strcmp(model,"C770UZ")) {
    height = 1718;
    width  = 2304;
    filters = 0x16161616;
    load_raw = &CLASS packed_load_raw;
    load_flags = 30;
  } else if (!strcmp(make,"OLYMPUS")) {
    height += height & 1;
    filters = exif_cfa;
    if (width == 4100) width -= 4;
    if (width == 4080) width -= 24;
    if (load_raw == &CLASS unpacked_load_raw)
      load_flags = 4;
    tiff_bps = 12;
    if (!strcmp(model,"E-300") ||
	!strcmp(model,"E-500")) {
      width -= 20;
      if (load_raw == &CLASS unpacked_load_raw) {
	maximum = 0xfc3;
	memset (cblack, 0, sizeof cblack);
      }
    } else if (!strcmp(model,"E-330")) {
      width -= 30;
      if (load_raw == &CLASS unpacked_load_raw)
	maximum = 0xf79;
    } else if (!strcmp(model,"SP550UZ")) {
      thumb_length = flen - (thumb_offset = 0xa39800);
      thumb_height = 480;
      thumb_width  = 640;
    }
  } else if (!strcmp(model,"N Digital")) {
    height = 2047;
    width  = 3072;
    filters = 0x61616161;
    data_offset = 0x1a00;
    load_raw = &CLASS packed_load_raw;
  } else if (!strcmp(model,"DSC-F828")) {
    width = 3288;
    left_margin = 5;
    data_offset = 862144;
    load_raw = &CLASS sony_load_raw;
    filters = 0x9c9c9c9c;
    colors = 4;
    strcpy (cdesc, "RGBE");
  } else if (!strcmp(model,"DSC-V3")) {
    width = 3109;
    left_margin = 59;
    data_offset = 787392;
    load_raw = &CLASS sony_load_raw;
  } else if (!strcmp(make,"SONY") && raw_width == 3984) {
    adobe_coeff ("SONY","DSC-R1");
    width = 3925;
    order = 0x4d4d;
  } else if (!strcmp(make,"SONY") && raw_width == 6048) {
    width -= 24;
  } else if (!strcmp(model,"DSLR-A100")) {
    if (width == 3880) {
      height--;
      width = ++raw_width;
    } else {
      order = 0x4d4d;
      load_flags = 2;
    }
    filters = 0x61616161;
  } else if (!strcmp(model,"DSLR-A350")) {
    height -= 4;
  } else if (!strcmp(model,"NEX-5N")) {
    width -= 24;
  } else if (!strcmp(model,"PIXL")) {
    height -= top_margin = 4;
    width -= left_margin = 32;
    gamma_curve (0, 7, 1, 255);
  } else if (!strcmp(model,"C603v")) {
    height = 480;
    width  = 640;
    if (fsize < 614400 || find_green (16, 16, 3840, 5120) < 25) goto c603v;
    strcpy (model,"KAI-0340");
    height -= 3;
    data_offset = 3840;
    order = 0x4949;
    load_raw = &CLASS unpacked_load_raw;
  } else if (!strcmp(model,"C603y")) {
    height = 2134;
    width  = 2848;
c603v:
    filters = 0;
    load_raw = &CLASS kodak_yrgb_load_raw;
    gamma_curve (0, 3.875, 1, 255);
  } else if (!strcmp(model,"C603")) {
    raw_height = height = 2152;
    raw_width  = width  = 2864;
    goto c603;
  } else if (!strcmp(model,"C330")) {
    height = 1744;
    width  = 2336;
    raw_height = 1779;
    raw_width  = 2338;
    top_margin = 33;
    left_margin = 1;
c603:
    order = 0x4949;
    if ((data_offset = fsize - raw_height*raw_width)) {
      fseek (ifp, 168, SEEK_SET);
      read_shorts (curve, 256);
    } else gamma_curve (0, 3.875, 1, 255);
    load_raw = &CLASS eight_bit_load_raw;
  } else if (!strncasecmp(model,"EasyShare",9)) {
    data_offset = data_offset < 0x15000 ? 0x15000 : 0x17000;
    load_raw = &CLASS packed_load_raw;
  } else if (!strcasecmp(make,"KODAK")) {
    if (filters == UINT_MAX) filters = 0x61616161;
    if (!strncmp(model,"NC2000",6)) {
      width -= 4;
      left_margin = 2;
    } else if (!strcmp(model,"EOSDCS3B")) {
      width -= 4;
      left_margin = 2;
    } else if (!strcmp(model,"EOSDCS1")) {
      width -= 4;
      left_margin = 2;
    } else if (!strcmp(model,"DCS420")) {
      width -= 4;
      left_margin = 2;
    } else if (!strncmp(model,"DCS460 ",7)) {
      model[6] = 0;
      width -= 4;
      left_margin = 2;
    } else if (!strcmp(model,"DCS460A")) {
      width -= 4;
      left_margin = 2;
      colors = 1;
      filters = 0;
    } else if (!strcmp(model,"DCS660M")) {
      black = 214;
      colors = 1;
      filters = 0;
    } else if (!strcmp(model,"DCS760M")) {
      colors = 1;
      filters = 0;
    }
    if (!strcmp(model+4,"20X"))
      strcpy (cdesc, "MYCY");
    if (strstr(model,"DC25")) {
      strcpy (model, "DC25");
      data_offset = 15424;
    }
    if (!strncmp(model,"DC2",3)) {
      height = 242;
      if (flen < 100000) {
	raw_width = 256; width = 249;
	pixel_aspect = (4.0*height) / (3.0*width);
      } else {
	raw_width = 512; width = 501;
	pixel_aspect = (493.0*height) / (373.0*width);
      }
      data_offset += raw_width + 1;
      colors = 4;
      filters = 0x8d8d8d8d;
      simple_coeff(1);
      pre_mul[1] = 1.179;
      pre_mul[2] = 1.209;
      pre_mul[3] = 1.036;
      load_raw = &CLASS eight_bit_load_raw;
    } else if (!strcmp(model,"40")) {
      strcpy (model, "DC40");
      height = 512;
      width  = 768;
      data_offset = 1152;
      load_raw = &CLASS kodak_radc_load_raw;
    } else if (strstr(model,"DC50")) {
      strcpy (model, "DC50");
      height = 512;
      width  = 768;
      data_offset = 19712;
      load_raw = &CLASS kodak_radc_load_raw;
    } else if (strstr(model,"DC120")) {
      strcpy (model, "DC120");
      height = 976;
      width  = 848;
      pixel_aspect = height/0.75/width;
      load_raw = tiff_compress == 7 ?
	&CLASS kodak_jpeg_load_raw : &CLASS kodak_dc120_load_raw;
    } else if (!strcmp(model,"DCS200")) {
      thumb_height = 128;
      thumb_width  = 192;
      thumb_offset = 6144;
      thumb_misc   = 360;
      write_thumb = &CLASS layer_thumb;
      height = 1024;
      width  = 1536;
      data_offset = 79872;
      load_raw = &CLASS eight_bit_load_raw;
      black = 17;
    }
  } else if (!strcmp(model,"Fotoman Pixtura")) {
    height = 512;
    width  = 768;
    data_offset = 3632;
    load_raw = &CLASS kodak_radc_load_raw;
    filters = 0x61616161;
    simple_coeff(2);
  } else if (!strncmp(model,"QuickTake",9)) {
    if (head[5]) strcpy (model+10, "200");
    fseek (ifp, 544, SEEK_SET);
    height = get2();
    width  = get2();
    data_offset = (get4(),get2()) == 30 ? 738:736;
    if (height > width) {
      SWAP(height,width);
      fseek (ifp, data_offset-6, SEEK_SET);
      flip = ~get2() & 3 ? 5:6;
    }
    filters = 0x61616161;
  } else if (!strcmp(make,"Rollei") && !load_raw) {
    switch (raw_width) {
      case 1316:
	height = 1030;
	width  = 1300;
	top_margin  = 1;
	left_margin = 6;
	break;
      case 2568:
	height = 1960;
	width  = 2560;
	top_margin  = 2;
	left_margin = 8;
    }
    filters = 0x16161616;
    load_raw = &CLASS rollei_load_raw;
  } else if (!strcmp(model,"PC-CAM 600")) {
    height = 768;
    data_offset = width = 1024;
    filters = 0x49494949;
    load_raw = &CLASS eight_bit_load_raw;
  } else if (!strcmp(model,"QV-2000UX")) {
    height = 1208;
    width  = 1632;
    data_offset = width * 2;
    load_raw = &CLASS eight_bit_load_raw;
  } else if (fsize == 3217760) {
    height = 1546;
    width  = 2070;
    raw_width = 2080;
    load_raw = &CLASS eight_bit_load_raw;
  } else if (!strcmp(model,"QV-4000")) {
    height = 1700;
    width  = 2260;
    load_raw = &CLASS unpacked_load_raw;
    maximum = 0xffff;
  } else if (!strcmp(model,"QV-5700")) {
    height = 1924;
    width  = 2576;
    raw_width = 3232;
    tiff_bps = 10;
  } else if (!strcmp(model,"QV-R41")) {
    height = 1720;
    width  = 2312;
    raw_width = 3520;
    left_margin = 2;
  } else if (!strcmp(model,"QV-R51")) {
    height = 1926;
    width  = 2580;
    raw_width = 3904;
  } else if (!strcmp(model,"EX-S20")) {
    height = 1208;
    width  = 1620;
    raw_width = 2432;
    flip = 3;
  } else if (!strcmp(model,"EX-S100")) {
    height = 1544;
    width  = 2058;
    raw_width = 3136;
  } else if (!strcmp(model,"EX-Z50")) {
    height = 1931;
    width  = 2570;
    raw_width = 3904;
  } else if (!strcmp(model,"EX-Z500")) {
    height = 1937;
    width  = 2577;
    raw_width = 3904;
    filters = 0x16161616;
  } else if (!strcmp(model,"EX-Z55")) {
    height = 1960;
    width  = 2570;
    raw_width = 3904;
  } else if (!strcmp(model,"EX-Z60")) {
    height = 2145;
    width  = 2833;
    raw_width = 3584;
    filters = 0x16161616;
    tiff_bps = 10;
  } else if (!strcmp(model,"EX-Z75")) {
    height = 2321;
    width  = 3089;
    raw_width = 4672;
    maximum = 0xfff;
  } else if (!strcmp(model,"EX-Z750")) {
    height = 2319;
    width  = 3087;
    raw_width = 4672;
    maximum = 0xfff;
  } else if (!strcmp(model,"EX-Z850")) {
    height = 2468;
    width  = 3279;
    raw_width = 4928;
    maximum = 0xfff;
  } else if (fsize == 15499264) {	/* EX-Z1050 or EX-Z1080 */
    height = 2752;
    width  = 3672;
    raw_width = 5632;
  } else if (!strcmp(model,"EX-P505")) {
    height = 1928;
    width  = 2568;
    raw_width = 3852;
    maximum = 0xfff;
  } else if (fsize == 9313536) {	/* EX-P600 or QV-R61 */
    height = 2142;
    width  = 2844;
    raw_width = 4288;
  } else if (!strcmp(model,"EX-P700")) {
    height = 2318;
    width  = 3082;
    raw_width = 4672;
  }
  if (!model[0])
    sprintf (model, "%dx%d", width, height);
  if (filters == UINT_MAX) filters = 0x94949494;
  if (raw_color) adobe_coeff (make, model);
  if (load_raw == &CLASS kodak_radc_load_raw)
    if (raw_color) adobe_coeff ("Apple","Quicktake");
  if (thumb_offset && !thumb_height) {
    fseek (ifp, thumb_offset, SEEK_SET);
    if (ljpeg_start (&jh, 1)) {
      thumb_width  = jh.wide;
      thumb_height = jh.high;
    }
  }
dng_skip:
  if (!tiff_bps) tiff_bps = 12;
  if (!maximum) maximum = (1 << tiff_bps) - 1;
  if (!load_raw || height < 22) is_raw = 0;
#ifndef HAVE_LIBJASPER
  if (load_raw == &CLASS redcine_load_raw) {
    dcraw_message (DCRAW_ERROR,_("%s: You must link dcraw with %s!!\n"),
	ifname_display, "libjasper");
    is_raw = 0;
  }
#endif
#ifndef HAVE_LIBJPEG
  if (load_raw == &CLASS kodak_jpeg_load_raw) {
    dcraw_message (DCRAW_ERROR,_("%s: You must link dcraw with %s!!\n"),
	ifname_display, "libjpeg");
    is_raw = 0;
  }
#endif
  if (!cdesc[0])
    strcpy (cdesc, colors == 3 ? "RGBG":"GMCY");
  if (!raw_height) raw_height = height;
  if (!raw_width ) raw_width  = width;
  if (filters && colors == 3)
    filters |= ((filters >> 2 & 0x22222222) |
		(filters << 2 & 0x88888888)) & filters << 1;
notraw:
  if (flip == -1) flip = tiff_flip;
  if (flip == -1) flip = 0;
}
