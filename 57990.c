static int mailimf_unstrict_crlf_parse(const char * message,
				       size_t length, size_t * indx)
{
  size_t cur_token;
  int r;

  cur_token = * indx;

  mailimf_cfws_parse(message, length, &cur_token);

  r = mailimf_char_parse(message, length, &cur_token, '\r');
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE))
    return r;

  r = mailimf_char_parse(message, length, &cur_token, '\n');
  if (r != MAILIMF_NO_ERROR)
    return r;

  * indx = cur_token;
  return MAILIMF_NO_ERROR;
}
