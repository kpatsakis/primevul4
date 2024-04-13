mailimf_number_parse(const char * message, size_t length,
		     size_t * indx, uint32_t * result)
{
  size_t cur_token;
  int digit;
  uint32_t number;
  int parsed;
  int r;

  cur_token = * indx;
  parsed = FALSE;

  number = 0;
  while (1) {
    r = mailimf_digit_parse(message, length, &cur_token, &digit);
    if (r != MAILIMF_NO_ERROR) {
      if (r == MAILIMF_ERROR_PARSE)
	break;
      else
	return r;
    }
    number *= 10;
    number += digit;
    parsed = TRUE;
  }

  if (!parsed)
    return MAILIMF_ERROR_PARSE;

  * result = number;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}
