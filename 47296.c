void CLASS parse_exif (int base)
{
  unsigned kodak, entries, tag, type, len, save, c;
  double expo;

  kodak = !strncmp(make,"EASTMAN",7) && tiff_nifds < 3;
  entries = get2();
  while (entries--) {
    tiff_get (base, &tag, &type, &len, &save);
    switch (tag) {
      case 33434:  shutter = getreal(type);		break;
      case 33437:  aperture = getreal(type);		break;
      case 34855:  iso_speed = get2();			break;
      case 36867:
      case 36868:  get_timestamp(0);			break;
      case 37377:  if ((expo = -getreal(type)) < 128)
		     shutter = pow (2, expo);		break;
      case 37378:  aperture = pow (2, getreal(type)/2);	break;
      case 37386:  focal_len = getreal(type);		break;
      case 37500:  parse_makernote (base, 0);		break;
      case 40962:  if (kodak) raw_width  = get4();	break;
      case 40963:  if (kodak) raw_height = get4();	break;
      case 41730:
	if (get4() == 0x20002)
	  for (exif_cfa=c=0; c < 8; c+=2)
	    exif_cfa |= fgetc(ifp) * 0x01010101 << c;
    }
    fseek (ifp, save, SEEK_SET);
  }
}
