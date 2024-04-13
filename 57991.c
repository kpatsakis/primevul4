static int mailimf_unstrict_msg_id_parse(const char * message, size_t length,
					 size_t * indx,
					 char ** result)
{
  char * msgid;
  size_t cur_token;
  int r;

  cur_token = * indx;

  r = mailimf_cfws_parse(message, length, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE))
    return r;

  r = mailimf_parse_unwanted_msg_id(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR)
    return r;

  r = mailimf_msg_id_parse(message, length, &cur_token, &msgid);
  if (r != MAILIMF_NO_ERROR)
    return r;

  r = mailimf_parse_unwanted_msg_id(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    free(msgid);
    return r;
  }

  * result = msgid;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}
