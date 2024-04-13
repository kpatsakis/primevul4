static inline int is_no_ws_ctl(char ch)
{
  if ((ch == 9) || (ch == 10) || (ch == 13))
    return FALSE;

  if (ch == 127)
     return TRUE;

  return (ch >= 1) && (ch <= 31);
}
