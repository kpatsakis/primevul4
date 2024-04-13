int mailimf_references_parse(const char * message, size_t length,
			     size_t * indx,
			     struct mailimf_references ** result)
{
  struct mailimf_references * references;
  size_t cur_token;
  clist * msg_id_list;
  int r;
  int res;

  cur_token = * indx;

  r = mailimf_token_case_insensitive_parse(message, length,
					   &cur_token, "References");
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_colon_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_msg_id_list_parse(message, length, &cur_token, &msg_id_list);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_unstrict_crlf_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_list;
  }

  references = mailimf_references_new(msg_id_list);
  if (references == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_list;
  }

  * result = references;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 free_list:
  clist_foreach(msg_id_list, (clist_func) mailimf_msg_id_free, NULL);
  clist_free(msg_id_list);
 err:
  return res;
}
