static inline int mailimf_comment_parse(const char * message, size_t length,
				 size_t * indx)
{
  size_t cur_token;
  int r;

  cur_token = * indx;

  r = mailimf_oparenth_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR)
    return r;

  while (1) {
    r = mailimf_comment_fws_ccontent_parse(message, length, &cur_token);
    if (r != MAILIMF_NO_ERROR) {
      if (r == MAILIMF_ERROR_PARSE)
	break;
      else
	return r;
    }
  }

  r = mailimf_fws_parse(message, length, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE))
    return r;

  r = mailimf_cparenth_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR)
    return r;

  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}
