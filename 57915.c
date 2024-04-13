int mailimf_cfws_parse(const char * message, size_t length,
		       size_t * indx)
{
  size_t cur_token;
  int has_comment;
  int r;

  cur_token = * indx;

  has_comment = FALSE;
  while (1) {
    r = mailimf_cfws_fws_comment_parse(message, length, &cur_token);
    if (r != MAILIMF_NO_ERROR) {
      if (r == MAILIMF_ERROR_PARSE)
	break;
      else
	return r;
    }
    has_comment = TRUE;
  }

  if (!has_comment) {
    r = mailimf_fws_parse(message, length, &cur_token);
    if (r != MAILIMF_NO_ERROR)
      return r;
  }

  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}
