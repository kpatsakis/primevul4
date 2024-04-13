static int mailimf_keywords_parse(const char * message, size_t length,
				  size_t * indx,
				  struct mailimf_keywords ** result)
{
  struct mailimf_keywords * keywords;
  clist * list;
  size_t cur_token;
  int r;
  int res;

  cur_token = * indx;

  r = mailimf_token_case_insensitive_parse(message, length,
					   &cur_token, "Keywords");
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_colon_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }
  
  r = mailimf_struct_list_parse(message, length, &cur_token,
				&list, ',',
				(mailimf_struct_parser *)
				mailimf_phrase_parse,
				(mailimf_struct_destructor *)
				mailimf_phrase_free);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_unstrict_crlf_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_list;
  }
  
  keywords = mailimf_keywords_new(list);
  if (keywords == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_list;
  }

  * result = keywords;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 free_list:
  clist_foreach(list, (clist_func) mailimf_phrase_free, NULL);
  clist_free(list);
 err:
  return res;
}
