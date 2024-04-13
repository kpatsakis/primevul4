static inline int mailimf_ccontent_parse(const char * message, size_t length,
					 size_t * indx)
{
  size_t cur_token;
  char ch;
  int r;
  
  cur_token = * indx;

  if (cur_token >= length)
    return MAILIMF_ERROR_PARSE;

  if (is_ctext(message[cur_token])) {
    cur_token ++;
  }
  else {
    r = mailimf_quoted_pair_parse(message, length, &cur_token, &ch);
    
    if (r == MAILIMF_ERROR_PARSE)
      r = mailimf_comment_parse(message, length, &cur_token);
    
    if (r == MAILIMF_ERROR_PARSE)
      return r;
  }

  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}
