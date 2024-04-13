MagickExport MagickBooleanType SetImageExtent(Image *image,const size_t columns,
  const size_t rows)
{
  if ((columns == 0) || (rows == 0))
    ThrowBinaryException(ImageError,"NegativeOrZeroImageSize",image->filename);
  image->columns=columns;
  image->rows=rows;
  if (image->depth > (8*sizeof(MagickSizeType)))
    ThrowBinaryException(ImageError,"ImageDepthNotSupported",image->filename);
  return(SyncImagePixelCache(image,&image->exception));
}
