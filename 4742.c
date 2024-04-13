 */
void php_wddx_packet_start(wddx_packet *packet, char *comment, size_t comment_len)
{
	php_wddx_add_chunk_static(packet, WDDX_PACKET_S);
	if (comment) {
		zend_string *escaped = php_escape_html_entities(
			comment, comment_len, 0, ENT_QUOTES, NULL);

		php_wddx_add_chunk_static(packet, WDDX_HEADER_S);
		php_wddx_add_chunk_static(packet, WDDX_COMMENT_S);
		php_wddx_add_chunk_ex(packet, ZSTR_VAL(escaped), ZSTR_LEN(escaped));
		php_wddx_add_chunk_static(packet, WDDX_COMMENT_E);
		php_wddx_add_chunk_static(packet, WDDX_HEADER_E);

		zend_string_release(escaped);
	} else {
		php_wddx_add_chunk_static(packet, WDDX_HEADER);
	}
	php_wddx_add_chunk_static(packet, WDDX_DATA_S);