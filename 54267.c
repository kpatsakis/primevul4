tcp_send_real (void *ssl, int sok, char *encoding, int using_irc, char *buf, int len)
{
	int ret;
	char *locale;
	gsize loc_len;

	if (encoding == NULL)	/* system */
	{
		locale = NULL;
		if (!prefs.utf8_locale)
		{
			const gchar *charset;

			g_get_charset (&charset);
			locale = g_convert_with_fallback (buf, len, charset, "UTF-8",
														 "?", 0, &loc_len, 0);
		}
	} else
	{
		if (using_irc)	/* using "IRC" encoding (CP1252/UTF-8 hybrid) */
			/* if all chars fit inside CP1252, use that. Otherwise this
			   returns NULL and we send UTF-8. */
			locale = g_convert (buf, len, "CP1252", "UTF-8", 0, &loc_len, 0);
		else
			locale = g_convert_with_fallback (buf, len, encoding, "UTF-8",
														 "?", 0, &loc_len, 0);
	}

	if (locale)
	{
		len = loc_len;
#ifdef USE_OPENSSL
		if (!ssl)
			ret = send (sok, locale, len, 0);
		else
			ret = _SSL_send (ssl, locale, len);
#else
		ret = send (sok, locale, len, 0);
#endif
		g_free (locale);
	} else
	{
#ifdef USE_OPENSSL
		if (!ssl)
			ret = send (sok, buf, len, 0);
		else
			ret = _SSL_send (ssl, buf, len);
#else
		ret = send (sok, buf, len, 0);
#endif
	}

	return ret;
}
