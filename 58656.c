static int prepend_unreachable(char **buffer, int *buflen)
{
	return prepend(buffer, buflen, "(unreachable)", 13);
}
