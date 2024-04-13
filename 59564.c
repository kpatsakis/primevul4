static unsigned short **DestroyPixelThreadSet(unsigned short **pixels)
{
  register ssize_t
    i;

  assert(pixels != (unsigned short **) NULL);
  for (i=0; i < (ssize_t) GetMagickResourceLimit(ThreadResource); i++)
    if (pixels[i] != (unsigned short *) NULL)
      pixels[i]=(unsigned short *) RelinquishMagickMemory(pixels[i]);
  pixels=(unsigned short **) RelinquishMagickMemory(pixels);
  return(pixels);
}
