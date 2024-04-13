static int mailimf_fws_atom_for_word_parse(const char * message, size_t length,
                                           size_t * indx, char ** result, int * p_missing_closing_quote)
{
  size_t end;
  size_t cur_token;
  int r;
  int res;
  struct mailmime_encoded_word * word;
  int has_fwd;
  int missing_closing_quote;
  char * atom;
  
  cur_token = * indx;
  missing_closing_quote = 0;
  
  r = mailimf_fws_parse(message, length, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE)) {
    res = r;
    goto err;
  }
  
  end = cur_token;
  
  r = mailmime_encoded_word_parse(message, length, &cur_token, &word, &has_fwd, &missing_closing_quote);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE)) {
    res = r;
    goto err;
  }
  
  if (r == MAILIMF_ERROR_PARSE) {
    return mailimf_fws_atom_parse(message, length, indx, result);
  }
  
  mailmime_encoded_word_free(word);
  
  atom = malloc(cur_token - end + 1);
  if (atom == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto err;
  }
  strncpy(atom, message + end, cur_token - end);
  atom[cur_token - end] = '\0';
  
  * result = atom;
  * indx = cur_token;
  * p_missing_closing_quote = missing_closing_quote;
  
  return MAILIMF_NO_ERROR;
  
err:
  return res;
}
