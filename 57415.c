MagickExport MagickBooleanType SetImageChannels(Image *image,
  const size_t channels)
{
  image->channels=channels;
  return(MagickTrue);
}
