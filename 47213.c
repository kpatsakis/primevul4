void CLASS bad_pixels (const char *cfname)
{
  FILE *fp=0;
  char *fname, *cp, line[128];
  int len, time, row, col, r, c, rad, tot, n, fixed=0;

  if (!filters) return;
  if (cfname)
    fp = fopen (cfname, "r");
  else {
    for (len=32 ; ; len *= 2) {
      fname = (char *) malloc (len);
      if (!fname) return;
      if (getcwd (fname, len-16)) break;
      free (fname);
      if (errno != ERANGE) return;
    }
#if defined(WIN32) || defined(DJGPP)
    if (fname[1] == ':')
      memmove (fname, fname+2, len-2);
    for (cp=fname; *cp; cp++)
      if (*cp == '\\') *cp = '/';
#endif
    cp = fname + strlen(fname);
    if (cp[-1] == '/') cp--;
    while (*fname == '/') {
      strcpy (cp, "/.badpixels");
      if ((fp = fopen (fname, "r"))) break;
      if (cp == fname) break;
      while (*--cp != '/');
    }
    free (fname);
  }
  if (!fp) return;
  while (::fgets (line, 128, fp)) {
    cp = strchr (line, '#');
    if (cp) *cp = 0;
    if (sscanf (line, "%d %d %d", &col, &row, &time) != 3) continue;
    if ((unsigned) col >= width || (unsigned) row >= height) continue;
    if (time > timestamp) continue;
    for (tot=n=0, rad=1; rad < 3 && n==0; rad++)
      for (r = row-rad; r <= row+rad; r++)
	for (c = col-rad; c <= col+rad; c++)
	  if ((unsigned) r < height && (unsigned) c < width &&
		(r != row || c != col) && fc(r,c) == fc(row,col)) {
	    tot += BAYER2(r,c);
	    n++;
	  }
    BAYER2(row,col) = tot/n;
    if (!fixed++) dcraw_message(DCRAW_VERBOSE,_("Fixed dead pixels at:"));
    dcraw_message(DCRAW_VERBOSE, " %d,%d", col, row);
  }
  if (fixed) dcraw_message(DCRAW_VERBOSE, "\n");
  fclose (fp);
}
