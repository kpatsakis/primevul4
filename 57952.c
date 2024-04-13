int mailimf_message_parse(const char * message, size_t length,
			  size_t * indx,
			  struct mailimf_message ** result)
{
  struct mailimf_fields * fields;
  struct mailimf_body * body;
  struct mailimf_message * msg;
  size_t cur_token;
  int r;
  int res;

  cur_token = * indx;

  r = mailimf_fields_parse(message, length, &cur_token, &fields);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_crlf_parse(message, length, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE)) {
    res = r;
    goto err;
  }

  r = mailimf_body_parse(message, length, &cur_token, &body);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_fields;
  }

  msg = mailimf_message_new(fields, body);
  if (msg == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_body;
  }

  * indx = cur_token;
  * result = msg;

  return MAILIMF_NO_ERROR;

 free_body:
  mailimf_body_free(body);
 free_fields:
  mailimf_fields_free(fields);
 err:
  return res;
}
