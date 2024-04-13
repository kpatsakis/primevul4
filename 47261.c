void CLASS jpeg_thumb()
{
  char *thumb;
  ushort exif[5];
  struct tiff_hdr th;

  thumb = (char *) malloc (thumb_length);
  merror (thumb, "jpeg_thumb()");
  fread (thumb, 1, thumb_length, ifp);
  fputc (0xff, ofp);
  fputc (0xd8, ofp);
  if (strcmp (thumb+6, "Exif")) {
    memcpy (exif, "\xff\xe1  Exif\0\0", 10);
    exif[1] = htons (8 + sizeof th);
    fwrite (exif, 1, sizeof exif, ofp);
    tiff_head (&th, 0);
    fwrite (&th, 1, sizeof th, ofp);
  }
  fwrite (thumb+2, 1, thumb_length-2, ofp);
  free (thumb);
}
