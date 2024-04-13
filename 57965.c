static int mailimf_phrase_parse(const char * message, size_t length,
				size_t * indx, char ** result)
{
  MMAPString * gphrase;
  char * word;
  int first;
  size_t cur_token;
  int r;
  int res;
  char * str;
  int has_missing_closing_quote;

  cur_token = * indx;
  has_missing_closing_quote = 0;

  gphrase = mmap_string_new("");
  if (gphrase == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto err;
  }

  first = TRUE;

  while (1) {
    int missing_quote = 0;
    r = mailimf_fws_word_parse(message, length, &cur_token, &word, &missing_quote);
    if (missing_quote) {
      has_missing_closing_quote = 1;
    }
    if (r == MAILIMF_NO_ERROR) {
      if (!first) {
	if (mmap_string_append_c(gphrase, ' ') == NULL) {
	  mailimf_word_free(word);
	  res = MAILIMF_ERROR_MEMORY;
	  goto free;
	}
      }
      if (mmap_string_append(gphrase, word) == NULL) {
	mailimf_word_free(word);
	res = MAILIMF_ERROR_MEMORY;
	goto free;
      }
      mailimf_word_free(word);
      first = FALSE;
    }
    else if (r == MAILIMF_ERROR_PARSE)
      break;
    else {
      res = r;
      goto free;
    }
  }

  if (first) {
    res = MAILIMF_ERROR_PARSE;
    goto free;
  }

  if (has_missing_closing_quote) {
    r = mailimf_char_parse(message, length, &cur_token, '\"');
  }

  str = strdup(gphrase->str);
  if (str == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free;
  }
  mmap_string_free(gphrase);

  * result = str;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 free:
  mmap_string_free(gphrase);
 err:
  return res;
}
