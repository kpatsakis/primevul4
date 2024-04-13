static inline int is_ftext(char ch)
{
  unsigned char uch = (unsigned char) ch;

  if (uch < 33)
    return FALSE;

  if (uch == 58)
    return FALSE;

  return TRUE;
}
