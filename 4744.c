   Creates a new packet and serializes the given value */
PHP_FUNCTION(wddx_serialize_value)
{
	zval *var;
	char *comment = NULL;
	size_t comment_len = 0;
	wddx_packet *packet;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|s", &var, &comment, &comment_len) == FAILURE) {
		return;
	}

	packet = php_wddx_constructor();

	php_wddx_packet_start(packet, comment, comment_len);
	php_wddx_serialize_var(packet, var, NULL);
	php_wddx_packet_end(packet);
	smart_str_0(packet);

	RETVAL_STR_COPY(packet->s);
	php_wddx_destructor(packet);