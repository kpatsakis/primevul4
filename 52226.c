get_line_size(const char *b, ssize_t avail, ssize_t *nlsize)
{
	ssize_t len;

	len = 0;
	while (len < avail) {
		switch (*b) {
		case '\0':/* Non-ascii character or control character. */
			if (nlsize != NULL)
				*nlsize = 0;
			return (-1);
		case '\r':
			if (avail-len > 1 && b[1] == '\n') {
				if (nlsize != NULL)
					*nlsize = 2;
				return (len+2);
			}
			/* FALL THROUGH */
		case '\n':
			if (nlsize != NULL)
				*nlsize = 1;
			return (len+1);
		default:
			b++;
			len++;
			break;
		}
	}
	if (nlsize != NULL)
		*nlsize = 0;
	return (avail);
}
