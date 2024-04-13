static int mailimf_qcontent_parse(const char * message, size_t length,
				  size_t * indx, char * result)
{
  size_t cur_token;
  char ch;
  int r;

  cur_token = * indx;

  if (cur_token >= length)
    return MAILIMF_ERROR_PARSE;

  if (is_qtext(message[cur_token])) {
    ch = message[cur_token];
    cur_token ++;
  }
  else {
    r = mailimf_quoted_pair_parse(message, length, &cur_token, &ch);
    
    if (r != MAILIMF_NO_ERROR)
      return r;
  }
  
  * result = ch;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}
