unpack_octal (GsfInfileTar *tar, const char *s, size_t len)
{
	guint64 res = 0;

	/*
	 * Different specifications differ on what terminating characters
	 * are allowed.  It doesn't hurt for us to allow both space and
	 * NUL.
	 */
	if (len == 0 || (s[len - 1] != 0 && s[len - 1] != ' '))
		goto invalid;
	len--;

	while (len--) {
		unsigned char c = *s++;
		if (c < '0' || c > '7')
			goto invalid;
		res = (res << 3) | (c - '0');
	}

	return (gint64)res;

invalid:
	tar->err = g_error_new (gsf_input_error_id (), 0,
				_("Invalid tar header"));
	return 0;
}
