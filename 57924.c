mailimf_custom_string_parse(const char * message, size_t length,
			    size_t * indx, char ** result,
			    int (* is_custom_char)(char))
{
  size_t begin;
  size_t end;
  char * gstr;

  begin = * indx;

  end = begin;

  if (end >= length)
    return MAILIMF_ERROR_PARSE;

  while (is_custom_char(message[end])) {
    end ++;
    if (end >= length)
      break;
  }

  if (end != begin) {
    /*
    gstr = strndup(message + begin, end - begin);
    */
    gstr = malloc(end - begin + 1);
    if (gstr == NULL)
      return MAILIMF_ERROR_MEMORY;
    strncpy(gstr, message + begin, end - begin);
    gstr[end - begin] = '\0';

    * indx = end;
    * result = gstr;
    return MAILIMF_NO_ERROR;
  }
  else
    return MAILIMF_ERROR_PARSE;
}
