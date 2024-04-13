static ssize_t WriteCALSRecord(Image *image,const char *data)
{
  char
    pad[128];

  register const char
    *p;

  register ssize_t
    i;

  ssize_t
    count;

  i=0;
  count=0;
  if (data != (const char *) NULL)
    {
      p=data;
      for (i=0; (i < 128) && (p[i] != '\0'); i++);
      count=WriteBlob(image,(size_t) i,(const unsigned char *) data);
    }
  if (i < 128)
    {
      i=128-i;
      (void) ResetMagickMemory(pad,' ',(size_t) i);
      count=WriteBlob(image,(size_t) i,(const unsigned char *) pad);
    }
  return(count);
}
