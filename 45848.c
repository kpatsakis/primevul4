wkbCollectionSubType(wkbObj *w)
{
  int t;
  memcpy(&t, (w->ptr + 1 + 4 + 4 + 1), sizeof(int));
  return wkbTypeMap(w,t);
}
