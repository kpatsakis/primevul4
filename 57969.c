static inline int mailimf_quoted_pair_parse(const char * message, size_t length,
					    size_t * indx, char * result)
{
  size_t cur_token;

  cur_token = * indx;
  
  if (cur_token + 1 >= length)
    return MAILIMF_ERROR_PARSE;

  if (message[cur_token] != '\\')
    return MAILIMF_ERROR_PARSE;

  cur_token ++;
  * result = message[cur_token];
  cur_token ++;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}
