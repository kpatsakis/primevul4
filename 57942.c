int mailimf_fws_quoted_string_parse(const char * message, size_t length,
				    size_t * indx, char ** result)
{
  size_t cur_token;
  MMAPString * gstr;
  char ch;
  char * str;
  int r;
  int res;

  cur_token = * indx;

  r = mailimf_fws_parse(message, length, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE)) {
    res = r;
    goto err;
  }

  r = mailimf_dquote_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  gstr = mmap_string_new("");
  if (gstr == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto err;
  }

#if 0
  if (mmap_string_append_c(gstr, '\"') == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_gstr;
  }
#endif

  while (1) {
    r = mailimf_fws_parse(message, length, &cur_token);
    if (r == MAILIMF_NO_ERROR) {
      if (mmap_string_append_c(gstr, ' ') == NULL) {
	res = MAILIMF_ERROR_MEMORY;
	goto free_gstr;
      }
    }
    else if (r != MAILIMF_ERROR_PARSE) {
      res = r;
      goto free_gstr;
    }

    r = mailimf_qcontent_parse(message, length, &cur_token, &ch);
    if (r == MAILIMF_NO_ERROR) {
      if (mmap_string_append_c(gstr, ch) == NULL) {
	res = MAILIMF_ERROR_MEMORY;
	goto free_gstr;
      }
    }
    else if (r == MAILIMF_ERROR_PARSE)
      break;
    else {
      res = r;
      goto free_gstr;
    }
  }

  r = mailimf_dquote_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_gstr;
  }

#if 0
  if (mmap_string_append_c(gstr, '\"') == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_gstr;
  }
#endif

  str = strdup(gstr->str);
  if (str == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_gstr;
  }
  mmap_string_free(gstr);

  * indx = cur_token;
  * result = str;

  return MAILIMF_NO_ERROR;

 free_gstr:
  mmap_string_free(gstr);
 err:
  return res;
}
