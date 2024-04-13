wkbReadInt(wkbObj *w)
{
  int i;
  memcpy(&i, w->ptr, sizeof(int));
  w->ptr += sizeof(int);
  return i;
}
