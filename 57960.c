static int mailimf_oparenth_parse(const char * message, size_t length,
				  size_t * indx)
{
  return mailimf_char_parse(message, length, indx, '(');
}
