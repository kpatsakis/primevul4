static int mailimf_angle_addr_parse(const char * message, size_t length,
				    size_t * indx, char ** result)
{
  size_t cur_token;
  char * addr_spec;
  int r;
  
  cur_token = * indx;
  
  r = mailimf_cfws_parse(message, length, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE))
    return r;
  
  r = mailimf_lower_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR)
    return r;
  
  r = mailimf_addr_spec_parse(message, length, &cur_token, &addr_spec);
  if (r != MAILIMF_NO_ERROR)
    return r;
  
  r = mailimf_greater_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    free(addr_spec);
    return r;
  }

  * result = addr_spec;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}
