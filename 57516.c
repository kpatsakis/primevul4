static int mem_is_in_memp(int mem, int num, UChar* memp)
{
  int i;
  MemNumType m;

  for (i = 0; i < num; i++) {
    GET_MEMNUM_INC(m, memp);
    if (mem == (int )m) return 1;
  }
  return 0;
}
