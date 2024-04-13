static MagickBooleanType WriteMipmaps(Image *image, const size_t pixelFormat,
  const size_t compression, const size_t mipmaps,
  const MagickBooleanType clusterFit, const MagickBooleanType weightByAlpha,
  ExceptionInfo *exception)
{
  Image*
    resize_image;

  register ssize_t
    i;

  size_t
    columns,
    rows;

  columns = image->columns;
  rows = image->rows;

  for (i=0; i< (ssize_t) mipmaps; i++)
  {
    resize_image = ResizeImage(image,DIV2(columns),DIV2(rows),TriangleFilter,1.0,
      exception);

    if (resize_image == (Image *) NULL)
      return(MagickFalse);

    DestroyBlob(resize_image);
    resize_image->blob=ReferenceBlob(image->blob);

    WriteImageData(resize_image,pixelFormat,compression,weightByAlpha,
      clusterFit,exception);

    resize_image=DestroyImage(resize_image);

    columns = DIV2(columns);
    rows = DIV2(rows);
  }

  return(MagickTrue);
}
