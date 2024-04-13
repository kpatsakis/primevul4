static int mailimf_parse_unwanted_msg_id(const char * message, size_t length,
					 size_t * indx)
{
  size_t cur_token;
  int r;
  char * word;
  int token_parsed;

  cur_token = * indx;

  token_parsed = TRUE;
  while (token_parsed) {
    token_parsed = FALSE;
    r = mailimf_word_parse(message, length, &cur_token, &word);
    if (r == MAILIMF_NO_ERROR) {
      mailimf_word_free(word);
      token_parsed = TRUE;
    }
    else if (r == MAILIMF_ERROR_PARSE) {
      /* do nothing */
    }
    else
      return r;
    r = mailimf_semi_colon_parse(message, length, &cur_token);
    if (r == MAILIMF_NO_ERROR)
      token_parsed = TRUE;
    else if (r == MAILIMF_ERROR_PARSE) {
      /* do nothing */
    }
    else
      return r;
    r = mailimf_comma_parse(message, length, &cur_token);
    if (r == MAILIMF_NO_ERROR)
      token_parsed = TRUE;
    else if (r == MAILIMF_ERROR_PARSE) {
      /* do nothing */
    }
    else
      return r;
    r = mailimf_plus_parse(message, length, &cur_token);
    if (r == MAILIMF_NO_ERROR)
      token_parsed = TRUE;
    else if (r == MAILIMF_ERROR_PARSE) {
      /* do nothing */
    }
    else
      return r;
    r = mailimf_colon_parse(message, length, &cur_token);
    if (r == MAILIMF_NO_ERROR)
      token_parsed = TRUE;
    else if (r == MAILIMF_ERROR_PARSE) {
      /* do nothing */
    }
    else
      return r;
    r = mailimf_point_parse(message, length, &cur_token);
    if (r == MAILIMF_NO_ERROR)
      token_parsed = TRUE;
    else if (r == MAILIMF_ERROR_PARSE) {
      /* do nothing */
    }
    else
      return r;
    r = mailimf_at_sign_parse(message, length, &cur_token);
    if (r == MAILIMF_NO_ERROR)
      token_parsed = TRUE;
    else if (r == MAILIMF_ERROR_PARSE) {
      /* do nothing */
    }
    else
      return r;
  }

  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}
