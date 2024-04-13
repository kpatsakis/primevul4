int mailimf_address_parse(const char * message, size_t length,
			  size_t * indx,
			  struct mailimf_address ** result)
{
  int type;
  size_t cur_token;
  struct mailimf_mailbox * mailbox;
  struct mailimf_group * group;
  struct mailimf_address * address;
  int r;
  int res;

  cur_token = * indx;

  mailbox = NULL;
  group = NULL;

  type = MAILIMF_ADDRESS_ERROR; /* XXX - removes a gcc warning */
  r = mailimf_group_parse(message, length, &cur_token, &group);
  if (r == MAILIMF_NO_ERROR)
    type = MAILIMF_ADDRESS_GROUP;
  
  if (r == MAILIMF_ERROR_PARSE) {
    r = mailimf_mailbox_parse(message, length, &cur_token, &mailbox);
    if (r == MAILIMF_NO_ERROR)
      type = MAILIMF_ADDRESS_MAILBOX;
  }

  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  address = mailimf_address_new(type, mailbox, group);
  if (address == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free;
  }

  * result = address;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;
  
 free:
  if (mailbox != NULL)
    mailimf_mailbox_free(mailbox);
  if (group != NULL)
    mailimf_group_free(group);
 err:
  return res;
}
