static inline signed short ReadProfileMSBShort(unsigned char **p,
  size_t *length)
{
  signed short
    value;

  if (*length < 2)
    return(0);
  value=ReadProfileShort(MSBEndian,*p);
  (*length)-=2;
  *p+=2;
  return(value);
}
