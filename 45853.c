wkbReadDouble(wkbObj *w)
{
  double d;
  memcpy(&d, w->ptr, sizeof(double));
  w->ptr += sizeof(double);
  return d;
}
