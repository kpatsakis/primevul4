int mailimf_char_parse(const char * message, size_t length,
		       size_t * indx, char token)
{
  size_t cur_token;

  cur_token = * indx;

  if (cur_token >= length)
    return MAILIMF_ERROR_PARSE;

  if (message[cur_token] == token) {
    cur_token ++;
    * indx = cur_token;
    return MAILIMF_NO_ERROR;
  }
  else
    return MAILIMF_ERROR_PARSE;
}
