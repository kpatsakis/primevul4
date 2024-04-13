static int php_tcp_sockop_set_option(php_stream *stream, int option, int value, void *ptrparam)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	php_stream_xport_param *xparam;

	switch(option) {
		case PHP_STREAM_OPTION_XPORT_API:
			xparam = (php_stream_xport_param *)ptrparam;

			switch(xparam->op) {
				case STREAM_XPORT_OP_CONNECT:
				case STREAM_XPORT_OP_CONNECT_ASYNC:
					xparam->outputs.returncode = php_tcp_sockop_connect(stream, sock, xparam);
					return PHP_STREAM_OPTION_RETURN_OK;

				case STREAM_XPORT_OP_BIND:
					xparam->outputs.returncode = php_tcp_sockop_bind(stream, sock, xparam);
					return PHP_STREAM_OPTION_RETURN_OK;


				case STREAM_XPORT_OP_ACCEPT:
					xparam->outputs.returncode = php_tcp_sockop_accept(stream, sock, xparam STREAMS_CC);
					return PHP_STREAM_OPTION_RETURN_OK;
				default:
					/* fall through */
					;
			}
	}
	return php_sockop_set_option(stream, option, value, ptrparam);
}
