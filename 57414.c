MagickExport MagickBooleanType ResetImagePage(Image *image,const char *page)
{
  MagickStatusType
    flags;

  RectangleInfo
    geometry;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  flags=ParseAbsoluteGeometry(page,&geometry);
  if ((flags & WidthValue) != 0)
    {
      if ((flags & HeightValue) == 0)
        geometry.height=geometry.width;
      image->page.width=geometry.width;
      image->page.height=geometry.height;
    }
  if ((flags & AspectValue) != 0)
    {
      if ((flags & XValue) != 0)
        image->page.x+=geometry.x;
      if ((flags & YValue) != 0)
        image->page.y+=geometry.y;
    }
  else
    {
      if ((flags & XValue) != 0)
        {
          image->page.x=geometry.x;
          if ((image->page.width == 0) && (geometry.x > 0))
            image->page.width=image->columns+geometry.x;
        }
      if ((flags & YValue) != 0)
        {
          image->page.y=geometry.y;
          if ((image->page.height == 0) && (geometry.y > 0))
            image->page.height=image->rows+geometry.y;
        }
    }
  return(MagickTrue);
}
