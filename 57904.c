mailimf_address_list_parse(const char * message, size_t length,
			   size_t * indx,
			   struct mailimf_address_list ** result)
{
  size_t cur_token;
  clist * list;
  struct mailimf_address_list * address_list;
  int r;
  int res;

  cur_token = * indx;

  r = mailimf_struct_list_parse(message, length,
				&cur_token, &list, ',',
				(mailimf_struct_parser *)
				mailimf_address_parse,
				(mailimf_struct_destructor *)
				mailimf_address_free);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  address_list = mailimf_address_list_new(list);
  if (address_list == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_list;
  }

  * result = address_list;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 free_list:
  clist_foreach(list, (clist_func) mailimf_address_free, NULL);
  clist_free(list);
 err:
  return res;
}				   
