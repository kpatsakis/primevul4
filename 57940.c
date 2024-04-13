int mailimf_fws_atom_parse(const char * message, size_t length,
			   size_t * indx, char ** result)
{
  size_t cur_token;
  int r;
  int res;
  char * atom;
  size_t end;

  cur_token = * indx;

  r = mailimf_fws_parse(message, length, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE)) {
    res = r;
    goto err;
  }

  end = cur_token;
  if (end >= length) {
    res = MAILIMF_ERROR_PARSE;
    goto err;
  }

  while (is_atext(message[end])) {
    end ++;
    if (end >= length)
      break;
  }
  if (end == cur_token) {
    res = MAILIMF_ERROR_PARSE;
    goto err;
  }

  atom = malloc(end - cur_token + 1);
  if (atom == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto err;
  }
  strncpy(atom, message + cur_token, end - cur_token);
  atom[end - cur_token] = '\0';

  cur_token = end;

  * indx = cur_token;
  * result = atom;

  return MAILIMF_NO_ERROR;

 err:
  return res;
}
