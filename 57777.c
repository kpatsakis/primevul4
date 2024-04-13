static void WriteImageData(Image *image, const size_t pixelFormat,
  const size_t compression, const MagickBooleanType clusterFit,
  const MagickBooleanType weightByAlpha, ExceptionInfo *exception)
{
  if (pixelFormat == DDPF_FOURCC)
    WriteFourCC(image,compression,clusterFit,weightByAlpha,exception);
  else
    WriteUncompressed(image,exception);
}
