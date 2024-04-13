_SSL_get_cipher_info (SSL * ssl)
{
	const SSL_CIPHER *c;


	c = SSL_get_current_cipher (ssl);
	safe_strcpy (chiper_info.version, SSL_CIPHER_get_version (c),
				sizeof (chiper_info.version));
	safe_strcpy (chiper_info.chiper, SSL_CIPHER_get_name (c),
				sizeof (chiper_info.chiper));
	SSL_CIPHER_get_bits (c, &chiper_info.chiper_bits);

	return (&chiper_info);
}
