   Starts a WDDX packet with optional comment and returns the packet id */
PHP_FUNCTION(wddx_packet_start)
{
	char *comment = NULL;
	size_t comment_len = 0;
	wddx_packet *packet;

	comment = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &comment, &comment_len) == FAILURE) {
		return;
	}

	packet = php_wddx_constructor();

	php_wddx_packet_start(packet, comment, comment_len);
	php_wddx_add_chunk_static(packet, WDDX_STRUCT_S);

	RETURN_RES(zend_register_resource(packet, le_wddx));