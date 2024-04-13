mailimf_envelope_and_optional_fields_parse(const char * message, size_t length,
					   size_t * indx,
					   struct mailimf_fields ** result)
{
  size_t cur_token;
  clist * list;
  struct mailimf_fields * fields;
  int r;
  int res;

  cur_token = * indx;

  list = NULL;

  r = mailimf_struct_multiple_parse(message, length, &cur_token,
				    &list,
				    (mailimf_struct_parser *)
				    mailimf_envelope_or_optional_field_parse,
				    (mailimf_struct_destructor *)
				    mailimf_field_free);
  switch (r) {
  case MAILIMF_NO_ERROR:
    /* do nothing */
    break;

  case MAILIMF_ERROR_PARSE:
    list = clist_new();
    if (list == NULL) {
      res = MAILIMF_ERROR_MEMORY;
      goto err;
    }
    break;

  default:
    res = r;
    goto err;
  }

  fields = mailimf_fields_new(list);
  if (fields == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free;
  }

  * result = fields;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 free:
  if (list != NULL) {
    clist_foreach(list, (clist_func) mailimf_field_free, NULL);
    clist_free(list);
  }
 err:
  return res;
}
