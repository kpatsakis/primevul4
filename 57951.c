static int mailimf_message_id_parse(const char * message, size_t length,
				    size_t * indx,
				    struct mailimf_message_id ** result)
{
  char * value;
  size_t cur_token;
  struct mailimf_message_id * message_id;
  int r;
  int res;

  cur_token = * indx;

  r = mailimf_token_case_insensitive_parse(message, length,
					   &cur_token, "Message-ID");
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_colon_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_msg_id_parse(message, length, &cur_token, &value);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_unstrict_crlf_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_value;
  }

  message_id = mailimf_message_id_new(value);
  if (message_id == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_value;
  }

  * result = message_id;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 free_value:
  mailimf_msg_id_free(value);
 err:
  return res;
}
