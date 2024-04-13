static int mailimf_in_reply_to_parse(const char * message, size_t length,
				     size_t * indx,
				     struct mailimf_in_reply_to ** result)
{
  struct mailimf_in_reply_to * in_reply_to;
  size_t cur_token;
  clist * msg_id_list;
  int res;
  int r;

  cur_token = * indx;

  r = mailimf_token_case_insensitive_parse(message, length,
					   &cur_token, "In-Reply-To");
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

  in_reply_to = mailimf_in_reply_to_new(msg_id_list);
  if (in_reply_to == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_list;
  }

  * result = in_reply_to;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 free_list:
  clist_foreach(msg_id_list, (clist_func) mailimf_msg_id_free, NULL);
  clist_free(msg_id_list);
 err:
  return res;
}
