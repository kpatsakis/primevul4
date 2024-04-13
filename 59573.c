static inline signed int ReadProfileMSBLong(unsigned char **p,size_t *length)
{
  signed int
    value;

  if (*length < 4)
    return(0);
  value=ReadProfileLong(MSBEndian,*p);
  (*length)-=4;
  *p+=4;
  return(value);
}
