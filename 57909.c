mailimf_bcc_parse(const char * message, size_t length,
		  size_t * indx, struct mailimf_bcc ** result)
{
  struct mailimf_address_list * addr_list;
  struct mailimf_bcc * bcc;
  size_t cur_token;
  int r;
  int res;

  cur_token = * indx;
  addr_list = NULL;

  r = mailimf_token_case_insensitive_parse(message, length,
					   &cur_token, "Bcc");
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
  switch (r) {
  case MAILIMF_NO_ERROR:
    /* do nothing */
    break;
  case MAILIMF_ERROR_PARSE:
    r = mailimf_cfws_parse(message, length, &cur_token);
    if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE)) {
      res = r;
      goto err;
    }
    break;
  default:
    res = r;
    goto err;
  }

  r = mailimf_unstrict_crlf_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  bcc = mailimf_bcc_new(addr_list);
  if (bcc == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto err;
  }

  * result = bcc;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 err:
  if (addr_list != NULL)
    mailimf_address_list_free(addr_list);
  return res;
}
