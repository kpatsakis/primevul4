int mailimf_ignore_field_parse(const char * message, size_t length,
			       size_t * indx)
{
  int has_field;
  size_t cur_token;
  int state;
  size_t terminal;

  has_field = FALSE;
  cur_token = * indx;

  terminal = cur_token;
  state = UNSTRUCTURED_START;

  /* check if this is not a beginning CRLF */

  if (cur_token >= length)
    return MAILIMF_ERROR_PARSE;

  switch (message[cur_token]) {
  case '\r':
    return MAILIMF_ERROR_PARSE;
  case '\n':
    return MAILIMF_ERROR_PARSE;
  }

  while (state != UNSTRUCTURED_OUT) {

    switch(state) {
    case UNSTRUCTURED_START:
      if (cur_token >= length)
	return MAILIMF_ERROR_PARSE;

      switch(message[cur_token]) {
      case '\r':
	state = UNSTRUCTURED_CR;
	break;
      case '\n':
	state = UNSTRUCTURED_LF;
	break;
      case ':':
	has_field = TRUE;
	state = UNSTRUCTURED_START;
	break;
      default:
	state = UNSTRUCTURED_START;
	break;
      }
      break;
    case UNSTRUCTURED_CR:
      if (cur_token >= length)
	return MAILIMF_ERROR_PARSE;

      switch(message[cur_token]) {
      case '\n':
	state = UNSTRUCTURED_LF;
	break;
      case ':':
	has_field = TRUE;
	state = UNSTRUCTURED_START;
	break;
      default:
	state = UNSTRUCTURED_START;
	break;
      }
      break;
    case UNSTRUCTURED_LF:
      if (cur_token >= length) {
	terminal = cur_token;
	state = UNSTRUCTURED_OUT;
	break;
      }

      switch(message[cur_token]) {
      case '\t':
      case ' ':
	state = UNSTRUCTURED_WSP;
	break;
      default:
	terminal = cur_token;
	state = UNSTRUCTURED_OUT;
	break;
      }
      break;
    case UNSTRUCTURED_WSP:
      if (cur_token >= length)
	return MAILIMF_ERROR_PARSE;

      switch(message[cur_token]) {
      case '\r':
	state = UNSTRUCTURED_CR;
	break;
      case '\n':
	state = UNSTRUCTURED_LF;
	break;
      case ':':
	has_field = TRUE;
	state = UNSTRUCTURED_START;
	break;
      default:
	state = UNSTRUCTURED_START;
	break;
      }
      break;
    }

    cur_token ++;
  }

  if (!has_field)
    return MAILIMF_ERROR_PARSE;

  * indx = terminal;

  return MAILIMF_NO_ERROR;
}
