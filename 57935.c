mailimf_envelope_or_optional_field_parse(const char * message,
					 size_t length,
					 size_t * indx,
					 struct mailimf_field ** result)
{
  int r;
  size_t cur_token;
  struct mailimf_optional_field * optional_field;
  struct mailimf_field * field;

  r = mailimf_envelope_field_parse(message, length, indx, result);
  if (r == MAILIMF_NO_ERROR)
    return MAILIMF_NO_ERROR;

  cur_token = * indx;

  r = mailimf_optional_field_parse(message, length, &cur_token,
				   &optional_field);
  if (r != MAILIMF_NO_ERROR)
    return r;

  field = mailimf_field_new(MAILIMF_FIELD_OPTIONAL_FIELD, NULL,
      NULL, NULL, NULL,
      NULL, NULL, NULL,
      NULL, NULL, NULL,
      NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, optional_field);
  if (field == NULL) {
    mailimf_optional_field_free(optional_field);
    return MAILIMF_ERROR_MEMORY;
  }

  * result = field;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}
