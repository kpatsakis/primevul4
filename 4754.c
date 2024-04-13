   Serializes given variables and adds them to packet given by packet_id */
PHP_FUNCTION(wddx_add_vars)
{
	int num_args, i;
	zval *args = NULL;
	zval *packet_id;
	wddx_packet *packet = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r+", &packet_id, &args, &num_args) == FAILURE) {
		return;
	}

	if ((packet = (wddx_packet *)zend_fetch_resource(Z_RES_P(packet_id), "WDDX packet ID", le_wddx)) == NULL) {
		RETURN_FALSE;
	}

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

	RETURN_TRUE;