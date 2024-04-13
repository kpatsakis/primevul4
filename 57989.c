int mailimf_unstrict_char_parse(const char * message, size_t length,
				size_t * indx, char token)
{
  size_t cur_token;
  int r;

  cur_token = * indx;

  r = mailimf_cfws_parse(message, length, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE))
    return r;

  r = mailimf_char_parse(message, length, &cur_token, token);
  if (r != MAILIMF_NO_ERROR)
    return r;
  
  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}
