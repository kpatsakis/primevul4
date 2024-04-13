int mailimf_envelope_fields_parse(const char * message, size_t length,
				  size_t * indx,
				  struct mailimf_fields ** result)
{
  size_t cur_token;
  clist * list;
  struct mailimf_fields * fields;
  int r;
  int res;

  cur_token = * indx;

  list = clist_new();
  if (list == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto err;
  }

  while (1) {
    struct mailimf_field * elt;

    r = mailimf_envelope_field_parse(message, length, &cur_token, &elt);
    if (r == MAILIMF_NO_ERROR) {
      r = clist_append(list, elt);
      if (r < 0) {
	res = MAILIMF_ERROR_MEMORY;
	goto free;
      }
    }
    else if (r == MAILIMF_ERROR_PARSE) {
      r = mailimf_ignore_field_parse(message, length, &cur_token);
      if (r == MAILIMF_NO_ERROR) {
	/* do nothing */
      }
      else if (r == MAILIMF_ERROR_PARSE) {
	break;
      }
      else {
	res = r;
	goto free;
      }
    }
    else {
      res = r;
      goto free;
    }
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
