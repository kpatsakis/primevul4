static int is_ascii_string(char* s, int len)
{
  int ret = 1, i = 0;
  for(i = 0; i < len; i++)
  {
      if ( !isascii( s[i] ) )
      {
          ret = 0;
          break;
      }
  }
  return ret;
}
