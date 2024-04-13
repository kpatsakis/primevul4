mailimf_struct_list_parse(const char * message, size_t length,
			  size_t * indx, clist ** result,
			  char symbol,
			  mailimf_struct_parser * parser,
			  mailimf_struct_destructor * destructor)
{
  clist * struct_list;
  size_t cur_token;
  void * value;
  size_t final_token;
  int r;
  int res;

  cur_token = * indx;

  r = parser(message, length, &cur_token, &value);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  struct_list = clist_new();
  if (struct_list == NULL) {
    destructor(value);
    res = MAILIMF_ERROR_MEMORY;
    goto err;
  }

  r = clist_append(struct_list, value);
  if (r < 0) {
    destructor(value);
    res = MAILIMF_ERROR_MEMORY;
    goto free;
  }

  final_token = cur_token;

  while (1) {
    r = mailimf_unstrict_char_parse(message, length, &cur_token, symbol);
    if (r != MAILIMF_NO_ERROR) {
      if (r == MAILIMF_ERROR_PARSE)
	break;
      else {
	res = r;
	goto free;
      }
    }

    r = parser(message, length, &cur_token, &value);
    if (r != MAILIMF_NO_ERROR) {
      if (r == MAILIMF_ERROR_PARSE)
	break;
      else {
	res = r;
	goto free;
      }
    }

    r = clist_append(struct_list, value);
    if (r < 0) {
      destructor(value);
      res = MAILIMF_ERROR_MEMORY;
      goto free;
    }

    final_token = cur_token;
  }
  
  * result = struct_list;
  * indx = final_token;
  
  return MAILIMF_NO_ERROR;
  
 free:
  clist_foreach(struct_list, (clist_func) destructor, NULL);
  clist_free(struct_list);
 err:
  return res;
}
