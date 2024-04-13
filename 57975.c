mailimf_resent_from_parse(const char * message, size_t length,
			  size_t * indx, struct mailimf_from ** result)
{
  struct mailimf_mailbox_list * mb_list;
  struct mailimf_from * from;
  size_t cur_token;
  int r;
  int res;

  cur_token =  * indx;

  r = mailimf_token_case_insensitive_parse(message, length,
					   &cur_token, "Resent-From");
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_colon_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_mailbox_list_parse(message, length, &cur_token, &mb_list);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_unstrict_crlf_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_mb_list;
  }

  from = mailimf_from_new(mb_list);
  if (from == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_mb_list;
  }

  * result = from;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 free_mb_list:
  mailimf_mailbox_list_free(mb_list);
 err:
  return res;
}
