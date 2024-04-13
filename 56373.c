static DoublePixelPacket **DestroyPixelThreadSet(DoublePixelPacket **pixels)
{
  register ssize_t
    i;

  assert(pixels != (DoublePixelPacket **) NULL);
  for (i=0; i < (ssize_t) GetMagickResourceLimit(ThreadResource); i++)
    if (pixels[i] != (DoublePixelPacket *) NULL)
      pixels[i]=(DoublePixelPacket *) RelinquishMagickMemory(pixels[i]);
  pixels=(DoublePixelPacket **) RelinquishMagickMemory(pixels);
  return(pixels);
}
