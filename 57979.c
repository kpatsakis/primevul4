static int mailimf_return_parse(const char * message, size_t length,
				size_t * indx,
				struct mailimf_return ** result)
{
  struct mailimf_path * path;
  struct mailimf_return * return_path;
  size_t cur_token;
  int r;
  int res;

  cur_token = * indx;

  r = mailimf_token_case_insensitive_parse(message, length,
					   &cur_token, "Return-Path");
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_colon_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  path = NULL;
  r = mailimf_path_parse(message, length, &cur_token, &path);
  if ( r!= MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_unstrict_crlf_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_path;
  }

  return_path = mailimf_return_new(path);
  if (return_path == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_path;
  }

  * result = return_path;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 free_path:
  mailimf_path_free(path);
 err:
  return res;
}
