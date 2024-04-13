static inline int is_atext(char ch)
{
  switch (ch) {
  case ' ':
  case '\t':
  case '\n':
  case '\r':
#if 0
  case '(':
  case ')':
#endif
  case '<':
  case '>':
#if 0
  case '@':
#endif
  case ',':
  case '"':
  case ':':
  case ';':
    return FALSE;
  default:
    return TRUE;
  }
}
