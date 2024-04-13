static inline int is_dtext(char ch)
{
  unsigned char uch = (unsigned char) ch;

  if (is_no_ws_ctl(ch))
    return TRUE;

  if (uch < 33)
    return FALSE;

  if ((uch >= 91) && (uch <= 93))
    return FALSE;

  if (uch == 127)
    return FALSE;

  return TRUE;
}
