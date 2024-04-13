static int mailimf_subject_parse(const char * message, size_t length,
				 size_t * indx,
				 struct mailimf_subject ** result)
{
  struct mailimf_subject * subject;
  char * value;
  size_t cur_token;
  int r;
  int res;

  cur_token = * indx;

  r = mailimf_token_case_insensitive_parse(message, length,
					   &cur_token, "Subject");
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_colon_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }
  
  r = mailimf_unstructured_parse(message, length, &cur_token, &value);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_unstrict_crlf_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_value;
  }
  
  subject = mailimf_subject_new(value);
  if (subject == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_value;
  }

  * result = subject;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 free_value:
  mailimf_unstructured_free(value);
 err:
  return res;
}
