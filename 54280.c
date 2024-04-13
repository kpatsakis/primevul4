_SSL_socket (SSL_CTX *ctx, int sd)
{
	SSL *ssl;


	if (!(ssl = SSL_new (ctx)))
		/* FATAL */
		__SSL_critical_error ("SSL_new");

	SSL_set_fd (ssl, sd);
	if (ctx->method == SSLv23_client_method())
		SSL_set_connect_state (ssl);
	else
	        SSL_set_accept_state(ssl);

	return (ssl);
}
