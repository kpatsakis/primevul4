static void WriteAlphas(Image *image, const ssize_t* alphas, size_t min5,
  size_t max5, size_t min7, size_t max7)
{
  register ssize_t
    i;

  size_t
    err5,
    err7,
    j;

  unsigned char
    indices5[16],
    indices7[16];

  FixRange(min5,max5,5);
  err5 = CompressAlpha(min5,max5,5,alphas,indices5);

  FixRange(min7,max7,7);
  err7 = CompressAlpha(min7,max7,7,alphas,indices7);

  if (err7 < err5)
  {
    for (i=0; i < 16; i++)
    {
      unsigned char
        index;

      index = indices7[i];
      if( index == 0 )
        indices5[i] = 1;
      else if (index == 1)
        indices5[i] = 0;
      else
        indices5[i] = 9 - index;
    }

    min5 = max7;
    max5 = min7;
  }
  
  (void) WriteBlobByte(image,(unsigned char) min5);
  (void) WriteBlobByte(image,(unsigned char) max5);
  
  for(i=0; i < 2; i++)
  {
    size_t
      value = 0;

    for (j=0; j < 8; j++)
    {
      size_t index = (size_t) indices5[j + i*8];
      value |= ( index << 3*j );
    }

    for (j=0; j < 3; j++)
    {
      size_t byte = (value >> 8*j) & 0xff;
      (void) WriteBlobByte(image,(unsigned char) byte);
    }
  }
}
