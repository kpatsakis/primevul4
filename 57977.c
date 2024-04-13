mailimf_resent_sender_parse(const char * message, size_t length,
			    size_t * indx, struct mailimf_sender ** result)
{
  struct mailimf_mailbox * mb;
  struct mailimf_sender * sender;
  size_t cur_token;
  int r;
  int res;

  cur_token = length;

  r = mailimf_token_case_insensitive_parse(message, length,
					   &cur_token, "Resent-Sender");
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_colon_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_mailbox_parse(message, length, &cur_token, &mb);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_unstrict_crlf_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_mb;
  }

  sender = mailimf_sender_new(mb);
  if (sender == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_mb;
  }

  * result = sender;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 free_mb:
  mailimf_mailbox_free(mb);
 err:
  return res;
}
