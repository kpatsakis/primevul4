static int mailimf_ignore_unstructured_parse(const char * message, size_t length,
					     size_t * indx)
{
  size_t cur_token;
  int state;
  size_t terminal;

  cur_token = * indx;

  state = UNSTRUCTURED_START;
  terminal = cur_token;

  while (state != UNSTRUCTURED_OUT) {

    switch(state) {
    case UNSTRUCTURED_START:
      if (cur_token >= length)
	return MAILIMF_ERROR_PARSE;
      terminal = cur_token;
      switch(message[cur_token]) {
      case '\r':
	state = UNSTRUCTURED_CR;
	break;
      case '\n':
	state = UNSTRUCTURED_LF;
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
      default:
	state = UNSTRUCTURED_START;
	break;
      }
      break;
    case UNSTRUCTURED_LF:
      if (cur_token >= length) {
	state = UNSTRUCTURED_OUT;
	break;
      }
      switch(message[cur_token]) {
      case '\t':
      case ' ':
	state = UNSTRUCTURED_WSP;
	break;
      default:
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
      default:
	state = UNSTRUCTURED_START;
	break;
      }
      break;
    }

    cur_token ++;
  }

  * indx = terminal;

  return MAILIMF_NO_ERROR;
}
