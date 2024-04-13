static int mailimf_second_parse(const char * message, size_t length,
				size_t * indx, int * result)
{
  uint32_t second;
  int r;

  r = mailimf_number_parse(message, length, indx, &second);
  if (r != MAILIMF_NO_ERROR)
    return r;

  * result = second;

  return MAILIMF_NO_ERROR;
}
