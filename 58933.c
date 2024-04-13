static inline int php_tcp_sockop_accept(php_stream *stream, php_netstream_data_t *sock,
		php_stream_xport_param *xparam STREAMS_DC)
{
	int clisock;

	xparam->outputs.client = NULL;

	clisock = php_network_accept_incoming(sock->socket,
			xparam->want_textaddr ? &xparam->outputs.textaddr : NULL,
			xparam->want_addr ? &xparam->outputs.addr : NULL,
			xparam->want_addr ? &xparam->outputs.addrlen : NULL,
			xparam->inputs.timeout,
			xparam->want_errortext ? &xparam->outputs.error_text : NULL,
			&xparam->outputs.error_code
			);

	if (clisock >= 0) {
		php_netstream_data_t *clisockdata;

		clisockdata = emalloc(sizeof(*clisockdata));

		if (clisockdata == NULL) {
			close(clisock);
			/* technically a fatal error */
		} else {
			memcpy(clisockdata, sock, sizeof(*clisockdata));
			clisockdata->socket = clisock;

			xparam->outputs.client = php_stream_alloc_rel(stream->ops, clisockdata, NULL, "r+");
			if (xparam->outputs.client) {
				xparam->outputs.client->ctx = stream->ctx;
				if (stream->ctx) {
					GC_REFCOUNT(stream->ctx)++;
				}
			}
		}
	}

	return xparam->outputs.client == NULL ? -1 : 0;
}
