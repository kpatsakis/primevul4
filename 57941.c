int mailimf_fws_parse(const char * message, size_t length, size_t * indx)
{
  size_t cur_token;
  size_t final_token;
  int fws_1;
  int fws_2;
  int fws_3;
  int r;
  
  cur_token = * indx;

  fws_1 = FALSE;
  while (1) {
    r = mailimf_wsp_parse(message, length, &cur_token);
    if (r != MAILIMF_NO_ERROR) {
      if (r == MAILIMF_ERROR_PARSE)
	break;
      else
	return r;
    }
    fws_1 = TRUE;
  }
  final_token = cur_token;

  r = mailimf_crlf_parse(message, length, &cur_token);
  switch (r) {
  case MAILIMF_NO_ERROR:
    fws_2 = TRUE;
    break;
  case MAILIMF_ERROR_PARSE:
    fws_2 = FALSE;
    break;
  default:
      return r;
  }
  
  fws_3 = FALSE;
  if (fws_2) {
    while (1) {
      r = mailimf_wsp_parse(message, length, &cur_token);
      if (r != MAILIMF_NO_ERROR) {
	if (r == MAILIMF_ERROR_PARSE)
	  break;
	else
	  return r;
      }
      fws_3 = TRUE;
    }
  }

  if ((!fws_1) && (!fws_3))
    return MAILIMF_ERROR_PARSE;

  if (!fws_3)
    cur_token = final_token;

  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}
