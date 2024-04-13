MagickExport MagickBooleanType IsTaintImage(const Image *image)
{
  char
    magick[MaxTextExtent],
    filename[MaxTextExtent];

  register const Image
    *p;

  assert(image != (Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image->signature == MagickSignature);
  (void) CopyMagickString(magick,image->magick,MaxTextExtent);
  (void) CopyMagickString(filename,image->filename,MaxTextExtent);
  for (p=image; p != (Image *) NULL; p=GetNextImageInList(p))
  {
    if (p->taint != MagickFalse)
      return(MagickTrue);
    if (LocaleCompare(p->magick,magick) != 0)
      return(MagickTrue);
    if (LocaleCompare(p->filename,filename) != 0)
      return(MagickTrue);
  }
  return(MagickFalse);
}
