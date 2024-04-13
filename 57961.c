mailimf_optional_field_parse(const char * message, size_t length,
			     size_t * indx,
			     struct mailimf_optional_field ** result)
{
  char * name;
  char * value;
  struct mailimf_optional_field * optional_field;
  size_t cur_token;
  int r;
  int res;

  cur_token = * indx;

  r = mailimf_field_name_parse(message, length, &cur_token, &name);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_colon_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_name;
  }

  r = mailimf_unstructured_parse(message, length, &cur_token, &value);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_name;
  }

  r = mailimf_unstrict_crlf_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_value;
  }

  optional_field = mailimf_optional_field_new(name, value);
  if (optional_field == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_value;
  }

  * result = optional_field;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 free_value:
  mailimf_unstructured_free(value);
 free_name:
  mailimf_field_name_free(name);
 err:
  return res;
}
