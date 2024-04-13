 */
static void php_wddx_serialize_number(wddx_packet *packet, zval *var)
{
	char tmp_buf[WDDX_BUF_LEN];
	zend_string *str = zval_get_string(var);
	snprintf(tmp_buf, sizeof(tmp_buf), WDDX_NUMBER, ZSTR_VAL(str));
	zend_string_release(str);

	php_wddx_add_chunk(packet, tmp_buf);