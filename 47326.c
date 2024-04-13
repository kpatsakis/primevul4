void CLASS rollei_thumb()
{
  unsigned i;
  ushort *thumb;

  thumb_length = thumb_width * thumb_height;
  thumb = (ushort *) calloc (thumb_length, 2);
  merror (thumb, "rollei_thumb()");
  fprintf (ofp, "P6\n%d %d\n255\n", thumb_width, thumb_height);
  read_shorts (thumb, thumb_length);
  for (i=0; i < thumb_length; i++) {
    putc (thumb[i] << 3, ofp);
    putc (thumb[i] >> 5  << 2, ofp);
    putc (thumb[i] >> 11 << 3, ofp);
  }
  free (thumb);
}
