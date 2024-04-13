static inline int mailimf_wsp_parse(const char * message, size_t length,
				    size_t * indx)
{
  size_t cur_token;

  cur_token = * indx;

  if (cur_token >= length)
    return MAILIMF_ERROR_PARSE;

  if ((message[cur_token] != ' ') && (message[cur_token] != '\t'))
    return MAILIMF_ERROR_PARSE;

  cur_token ++;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}
