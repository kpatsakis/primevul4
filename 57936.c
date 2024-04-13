static int mailimf_field_name_parse(const char * message, size_t length,
				    size_t * indx, char ** result)
{
  char * field_name;
  size_t cur_token;
  size_t end;
  
  cur_token = * indx;

  end = cur_token;
  if (end >= length) {
    return MAILIMF_ERROR_PARSE;
  }

  while (is_ftext(message[end])) {
    end ++;
    if (end >= length)
      break;
  }
  if (end == cur_token) {
    return MAILIMF_ERROR_PARSE;
  }

  /*  field_name = strndup(message + cur_token, end - cur_token); */
  field_name = malloc(end - cur_token + 1);
  if (field_name == NULL) {
    return MAILIMF_ERROR_MEMORY;
  }
  strncpy(field_name, message + cur_token, end - cur_token);
  field_name[end - cur_token] = '\0';

  cur_token = end;
  
  * indx = cur_token;
  * result = field_name;
  
  return MAILIMF_NO_ERROR;
}
