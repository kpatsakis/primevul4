static int mailimf_digit_parse(const char * message, size_t length,
			       size_t * indx, int * result)
{
  size_t cur_token;

  cur_token = * indx;
  
  if (cur_token >= length)
    return MAILIMF_ERROR_PARSE;

  if (is_digit(message[cur_token])) {
    * result = message[cur_token] - '0';
    cur_token ++;
    * indx = cur_token;
    return MAILIMF_NO_ERROR;
  }
  else
    return MAILIMF_ERROR_PARSE;
}
