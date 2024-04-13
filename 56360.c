static MagickBooleanType ReadRectangle(Image *image,PICTRectangle *rectangle)
{
  rectangle->top=(short) ReadBlobMSBShort(image);
  rectangle->left=(short) ReadBlobMSBShort(image);
  rectangle->bottom=(short) ReadBlobMSBShort(image);
  rectangle->right=(short) ReadBlobMSBShort(image);
  if ((EOFBlob(image) != MagickFalse) || (rectangle->left > rectangle->right) ||
      (rectangle->top > rectangle->bottom))
    return(MagickFalse);
  return(MagickTrue);
}
