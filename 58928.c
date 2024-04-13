static int php_sockop_flush(php_stream *stream)
{
#if 0
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	return fsync(sock->socket);
#endif
	return 0;
}
