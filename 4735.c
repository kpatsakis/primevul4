   Ends specified WDDX packet and returns the string containing the packet */
PHP_FUNCTION(wddx_packet_end)
{
	zval *packet_id;
	wddx_packet *packet = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &packet_id) == FAILURE) {
		return;
	}

	if ((packet = (wddx_packet *)zend_fetch_resource(Z_RES_P(packet_id), "WDDX packet ID", le_wddx)) == NULL) {
		RETURN_FALSE;
	}

	php_wddx_add_chunk_static(packet, WDDX_STRUCT_E);

	php_wddx_packet_end(packet);
	smart_str_0(packet);

	RETVAL_STR_COPY(packet->s);

	zend_list_close(Z_RES_P(packet_id));