   Creates a new packet and serializes given variables into a struct */
PHP_FUNCTION(wddx_serialize_vars)
{
	int num_args, i;
	wddx_packet *packet;
	zval *args = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "+", &args, &num_args) == FAILURE) {
		return;
	}

	packet = php_wddx_constructor();

	php_wddx_packet_start(packet, NULL, 0);
	php_wddx_add_chunk_static(packet, WDDX_STRUCT_S);

	for (i=0; i<num_args; i++) {
		zval *arg;
		if (!Z_ISREF(args[i])) {
			arg = &args[i];
		} else {
			arg = Z_REFVAL(args[i]);
		}
		if (Z_TYPE_P(arg) != IS_ARRAY && Z_TYPE_P(arg) != IS_OBJECT) {
			convert_to_string_ex(arg);
		}
		php_wddx_add_var(packet, arg);
	}

	php_wddx_add_chunk_static(packet, WDDX_STRUCT_E);
	php_wddx_packet_end(packet);
	smart_str_0(packet);

	RETVAL_STR_COPY(packet->s);
	php_wddx_destructor(packet);