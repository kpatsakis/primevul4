void CLASS parse_external_jpeg()
{
  const char *file, *ext;
  char *jname, *jfile, *jext;
  FILE *save=ifp;

  ext  = strrchr (ifname, '.');
  file = strrchr (ifname, '/');
  if (!file) file = strrchr (ifname, '\\');
  if (!file) file = ifname-1;
  file++;
  if (!ext || strlen(ext) != 4 || ext-file != 8) return;
  jname = (char *) malloc (strlen(ifname) + 1);
  merror (jname, "parse_external_jpeg()");
  strcpy (jname, ifname);
  jfile = file - ifname + jname;
  jext  = ext  - ifname + jname;
  if (strcasecmp (ext, ".jpg")) {
    strcpy (jext, isupper(ext[1]) ? ".JPG":".jpg");
    if (isdigit(*file)) {
      memcpy (jfile, file+4, 4);
      memcpy (jfile+4, file, 4);
    }
  } else
    while (isdigit(*--jext)) {
      if (*jext != '9') {
	(*jext)++;
	break;
      }
      *jext = '0';
    }
  if (strcmp (jname, ifname)) {
    if ((ifp = fopen (jname, "rb"))) {
      dcraw_message (DCRAW_VERBOSE,_("Reading metadata from %s ...\n"), jname);
      parse_tiff (12);
      thumb_offset = 0;
      is_raw = 1;
      fclose (ifp);
    }
  }
  if (!timestamp)
    dcraw_message (DCRAW_WARNING,_("Failed to read metadata from %s\n"), jname);
  free (jname);
  ifp = save;
}
