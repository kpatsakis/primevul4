static int mailimf_display_name_parse(const char * message, size_t length,
				      size_t * indx, char ** result)
{
  return mailimf_phrase_parse(message, length, indx, result);
}
