MagickExport MagickBooleanType ClipImage(Image *image)
{
  return(ClipImagePath(image,"#1",MagickTrue));
}
