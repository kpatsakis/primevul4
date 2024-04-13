mailimf_resent_bcc_parse(const char * message, size_t length,
			 size_t * indx, struct mailimf_bcc ** result)
{
  struct mailimf_address_list * addr_list;
  struct mailimf_bcc * bcc;
  size_t cur_token;
  int r;
  int res;

  cur_token = * indx;
  bcc = NULL;

  r = mailimf_token_case_insensitive_parse(message, length,
					   &cur_token, "Resent-Bcc");
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_colon_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  addr_list = NULL;
  r = mailimf_address_list_parse(message, length, &cur_token, &addr_list);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE)) {
    res = r;
    goto err;
  }

  r = mailimf_unstrict_crlf_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_addr_list;
  }

  bcc = mailimf_bcc_new(addr_list);
  if (bcc == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_addr_list;
  }

  * result = bcc;
  * indx = cur_token;

  return TRUE;

 free_addr_list:
  mailimf_address_list_free(addr_list);
 err:
  return res;
}
