static inline int ProfileInteger(Image *image,short int *hex_digits)
{
  int
    c,
    l,
    value;

  register ssize_t
    i;

  l=0;
  value=0;
  for (i=0; i < 2; )
  {
    c=ReadBlobByte(image);
    if ((c == EOF) || ((c == '%') && (l == '%')))
      {
        value=(-1);
        break;
      }
    l=c;
    c&=0xff;
    if (isxdigit(c) == MagickFalse)
      continue;
    value=(int) ((size_t) value << 4)+hex_digits[c];
    i++;
  }
  return(value);
}
