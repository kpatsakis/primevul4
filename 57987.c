mailimf_to_parse(const char * message, size_t length,
		 size_t * indx, struct mailimf_to ** result)
{
  struct mailimf_address_list * addr_list;
  struct mailimf_to * to;
  size_t cur_token;
  int r;
  int res;

  cur_token = * indx;

  r = mailimf_token_case_insensitive_parse(message, length,
					   &cur_token, "To");
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_colon_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_address_list_parse(message, length, &cur_token, &addr_list);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_unstrict_crlf_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_addr_list;
  }

  to = mailimf_to_new(addr_list);
  if (to == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_addr_list;
  }

  * result = to;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 free_addr_list:
  mailimf_address_list_free(addr_list);
 err:
  return res;
}
