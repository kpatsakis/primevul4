static void strstore(char **str, const char *newstr)
{
  if(*str)
    free(*str);
  *str = strdup(newstr);
}
