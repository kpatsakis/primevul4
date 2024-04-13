int mailimf_body_parse(const char * message, size_t length,
		       size_t * indx,
		       struct mailimf_body ** result)
{
  size_t cur_token;
  struct mailimf_body * body;

  cur_token = * indx;

  body = mailimf_body_new(message + cur_token, length - cur_token);
  if (body == NULL)
    return MAILIMF_ERROR_MEMORY;

  cur_token = length;

  * result = body;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}
