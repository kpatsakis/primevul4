void CLASS ppm_thumb()
{
  char *thumb;
  thumb_length = thumb_width*thumb_height*3;
  thumb = (char *) malloc (thumb_length);
  merror (thumb, "ppm_thumb()");
  fprintf (ofp, "P6\n%d %d\n255\n", thumb_width, thumb_height);
  fread  (thumb, 1, thumb_length, ifp);
  fwrite (thumb, 1, thumb_length, ofp);
  free (thumb);
}
