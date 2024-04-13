static int TIFFCloseBlob(thandle_t image)
{
  (void) CloseBlob((Image *) image);
  return(0);
}
