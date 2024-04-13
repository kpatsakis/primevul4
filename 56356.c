static size_t EncodeImage(Image *image,const unsigned char *scanline,
  const size_t bytes_per_line,unsigned char *pixels)
{
#define MaxCount  128
#define MaxPackbitsRunlength  128

  register const unsigned char
    *p;

  register ssize_t
    i;

  register unsigned char
    *q;

  size_t
    length;

  ssize_t
    count,
    repeat_count,
    runlength;

  unsigned char
    index;

  /*
    Pack scanline.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(scanline != (unsigned char *) NULL);
  assert(pixels != (unsigned char *) NULL);
  count=0;
  runlength=0;
  p=scanline+(bytes_per_line-1);
  q=pixels;
  index=(*p);
  for (i=(ssize_t) bytes_per_line-1; i >= 0; i--)
  {
    if (index == *p)
      runlength++;
    else
      {
        if (runlength < 3)
          while (runlength > 0)
          {
            *q++=(unsigned char) index;
            runlength--;
            count++;
            if (count == MaxCount)
              {
                *q++=(unsigned char) (MaxCount-1);
                count-=MaxCount;
              }
          }
        else
          {
            if (count > 0)
              *q++=(unsigned char) (count-1);
            count=0;
            while (runlength > 0)
            {
              repeat_count=runlength;
              if (repeat_count > MaxPackbitsRunlength)
                repeat_count=MaxPackbitsRunlength;
              *q++=(unsigned char) index;
              *q++=(unsigned char) (257-repeat_count);
              runlength-=repeat_count;
            }
          }
        runlength=1;
      }
    index=(*p);
    p--;
  }
  if (runlength < 3)
    while (runlength > 0)
    {
      *q++=(unsigned char) index;
      runlength--;
      count++;
      if (count == MaxCount)
        {
          *q++=(unsigned char) (MaxCount-1);
          count-=MaxCount;
        }
    }
  else
    {
      if (count > 0)
        *q++=(unsigned char) (count-1);
      count=0;
      while (runlength > 0)
      {
        repeat_count=runlength;
        if (repeat_count > MaxPackbitsRunlength)
          repeat_count=MaxPackbitsRunlength;
        *q++=(unsigned char) index;
        *q++=(unsigned char) (257-repeat_count);
        runlength-=repeat_count;
      }
    }
  if (count > 0)
    *q++=(unsigned char) (count-1);
  /*
    Write the number of and the packed length.
  */
  length=(size_t) (q-pixels);
  if (bytes_per_line > 200)
    {
      (void) WriteBlobMSBShort(image,(unsigned short) length);
      length+=2;
    }
  else
    {
      (void) WriteBlobByte(image,(unsigned char) length);
      length++;
    }
  while (q != pixels)
  {
    q--;
    (void) WriteBlobByte(image,*q);
  }
  return(length);
}
