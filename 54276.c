_SSL_context_init (void (*info_cb_func), int server)
{
	SSL_CTX *ctx;
#ifdef WIN32
	int i, r;
#endif

	SSLeay_add_ssl_algorithms ();
	SSL_load_error_strings ();
	ctx = SSL_CTX_new (server ? SSLv23_server_method() : SSLv23_client_method ());

	SSL_CTX_set_session_cache_mode (ctx, SSL_SESS_CACHE_BOTH);
	SSL_CTX_set_timeout (ctx, 300);
	SSL_CTX_set_options (ctx, SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3
							  |SSL_OP_NO_COMPRESSION
							  |SSL_OP_SINGLE_DH_USE|SSL_OP_SINGLE_ECDH_USE
							  |SSL_OP_NO_TICKET
							  |SSL_OP_CIPHER_SERVER_PREFERENCE);

	/* used in SSL_connect(), SSL_accept() */
	SSL_CTX_set_info_callback (ctx, info_cb_func);

#ifdef WIN32
	/* under win32, OpenSSL needs to be seeded with some randomness */
	for (i = 0; i < 128; i++)
	{
		r = rand ();
		RAND_seed ((unsigned char *)&r, sizeof (r));
	}
#endif

	return(ctx);
}
