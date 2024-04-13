 */
static void php_wddx_serialize_string(wddx_packet *packet, zval *var)
{
	php_wddx_add_chunk_static(packet, WDDX_STRING_S);

	if (Z_STRLEN_P(var) > 0) {
		zend_string *buf = php_escape_html_entities(
			(unsigned char *) Z_STRVAL_P(var), Z_STRLEN_P(var), 0, ENT_QUOTES, NULL);

		php_wddx_add_chunk_ex(packet, ZSTR_VAL(buf), ZSTR_LEN(buf));

		zend_string_release(buf);
	}
	php_wddx_add_chunk_static(packet, WDDX_STRING_E);