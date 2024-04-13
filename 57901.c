static inline int is_qtext(char ch)
{
  unsigned char uch = (unsigned char) ch;

  if (is_no_ws_ctl(ch))
    return TRUE;

  if (uch < 33)
    return FALSE;

  if (uch == 34)
    return FALSE;

  if (uch == 92)
    return FALSE;

  if (uch == 127)
    return FALSE;

  return TRUE;
}
