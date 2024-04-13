int mailimf_mailbox_parse(const char * message, size_t length,
			  size_t * indx,
			  struct mailimf_mailbox ** result)
{
  size_t cur_token;
  char * display_name;
  struct mailimf_mailbox * mailbox;
  char * addr_spec;
  int r;
  int res;

  cur_token = * indx;
  display_name = NULL;
  addr_spec = NULL;

  r = mailimf_name_addr_parse(message, length, &cur_token,
			      &display_name, &addr_spec);
  if (r == MAILIMF_ERROR_PARSE)
    r = mailimf_addr_spec_parse(message, length, &cur_token, &addr_spec);

  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  mailbox = mailimf_mailbox_new(display_name, addr_spec);
  if (mailbox == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free;
  }

  * result = mailbox;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 free:
  if (display_name != NULL)
    mailimf_display_name_free(display_name);
  if (addr_spec != NULL)
    mailimf_addr_spec_free(addr_spec);
 err:
  return res;
}
