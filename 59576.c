static inline const unsigned char *ReadResourceByte(const unsigned char *p,
  unsigned char *quantum)
{
  *quantum=(*p++);
  return(p);
}
