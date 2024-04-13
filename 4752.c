   Deserializes given packet and returns a PHP value */
PHP_FUNCTION(wddx_deserialize)
{
	zval *packet;
	php_stream *stream = NULL;
	zend_string *payload = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &packet) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(packet) == IS_STRING) {
		payload = Z_STR_P(packet);
	} else if (Z_TYPE_P(packet) == IS_RESOURCE) {
		php_stream_from_zval(stream, packet);
		if (stream) {
			payload = php_stream_copy_to_mem(stream, PHP_STREAM_COPY_ALL, 0);
		}
	} else {
		php_error_docref(NULL, E_WARNING, "Expecting parameter 1 to be a string or a stream");
		return;
	}

	if (payload == NULL) {
		return;
	}

	php_wddx_deserialize_ex(ZSTR_VAL(payload), ZSTR_LEN(payload), return_value);

	if (stream) {
		efree(payload);
	}