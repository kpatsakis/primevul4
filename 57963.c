mailimf_orig_date_parse(const char * message, size_t length,
			size_t * indx, struct mailimf_orig_date ** result)
{
  struct mailimf_date_time * date_time;
  struct mailimf_orig_date * orig_date;
  size_t cur_token;
  int r;
  int res;

  cur_token = * indx;

  r = mailimf_token_case_insensitive_parse(message, length,
					   &cur_token, "Date:");
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_date_time_parse(message, length, &cur_token, &date_time);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_ignore_unstructured_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_date_time;
  }

  r = mailimf_unstrict_crlf_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_date_time;
  }

  orig_date = mailimf_orig_date_new(date_time);
  if (orig_date == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_date_time;
  }

  * result = orig_date;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 free_date_time:
  mailimf_date_time_free(date_time);
 err:
  return res;
}
