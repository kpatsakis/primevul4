static int mailimf_unstructured_parse(const char * message, size_t length,
				      size_t * indx, char ** result)
{
  size_t cur_token;
  int state;
  size_t begin;
  size_t terminal;
  char * str;

  cur_token = * indx;


  while (1) {
    int r;

    r = mailimf_wsp_parse(message, length, &cur_token);
    if (r == MAILIMF_NO_ERROR) {
      /* do nothing */
    }
    else if (r == MAILIMF_ERROR_PARSE)
      break;
    else {
      return r;
    }
  }

  state = UNSTRUCTURED_START;
  begin = cur_token;
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

  str = malloc(terminal - begin + 1);
  if (str == NULL)
    return MAILIMF_ERROR_MEMORY;
  strncpy(str, message + begin,  terminal - begin);
  str[terminal - begin] = '\0';

  * indx = terminal;
  * result = str;

  return MAILIMF_NO_ERROR;
}
