void CLASS parse_riff()
{
  unsigned i, size, end;
  char tag[4], date[64], month[64];
  static const char mon[12][4] =
  { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };
  struct tm t;

  order = 0x4949;
  fread (tag, 4, 1, ifp);
  size = get4();
  end = ftell(ifp) + size;
  if (!memcmp(tag,"RIFF",4) || !memcmp(tag,"LIST",4)) {
    get4();
    while ((unsigned)(ftell(ifp)+7) < end)
      parse_riff();
  } else if (!memcmp(tag,"nctg",4)) {
    while ((unsigned)(ftell(ifp)+7) < end) {
      i = get2();
      size = get2();
      if ((i+1) >> 1 == 10 && size == 20)
	get_timestamp(0);
      else fseek (ifp, size, SEEK_CUR);
    }
  } else if (!memcmp(tag,"IDIT",4) && size < 64) {
    fread (date, 64, 1, ifp);
    date[size] = 0;
    memset (&t, 0, sizeof t);
    if (sscanf (date, "%*s %s %d %d:%d:%d %d", month, &t.tm_mday,
	&t.tm_hour, &t.tm_min, &t.tm_sec, &t.tm_year) == 6) {
      for (i=0; i < 12 && strcasecmp(mon[i],month); i++);
      t.tm_mon = i;
      t.tm_year -= 1900;
      if (mktime(&t) > 0)
	timestamp = mktime(&t);
    }
  } else
    fseek (ifp, size, SEEK_CUR);
}
