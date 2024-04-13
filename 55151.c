static ssize_t ReadBlobBlock(Image *image,unsigned char *data)
{
  ssize_t
    count;

  unsigned char
    block_count;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(data != (unsigned char *) NULL);
  count=ReadBlob(image,1,&block_count);
  if (count != 1)
    return(0);
  count=ReadBlob(image,(size_t) block_count,data);
  if (count != (ssize_t) block_count)
    return(0);
  return(count);
}
