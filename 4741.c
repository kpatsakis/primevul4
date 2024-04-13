 */
static void php_wddx_serialize_object(wddx_packet *packet, zval *obj)
{
/* OBJECTS_FIXME */
	zval *ent, fname, *varname;
	zval retval;
	zend_string *key;
	zend_ulong idx;
	char tmp_buf[WDDX_BUF_LEN];
	HashTable *objhash, *sleephash;

	ZVAL_STRING(&fname, "__sleep");
	/*
	 * We try to call __sleep() method on object. It's supposed to return an
	 * array of property names to be serialized.
	 */
	if (call_user_function_ex(CG(function_table), obj, &fname, &retval, 0, 0, 1, NULL) == SUCCESS) {
		if (!Z_ISUNDEF(retval) && (sleephash = HASH_OF(&retval))) {
			PHP_CLASS_ATTRIBUTES;

			PHP_SET_CLASS_ATTRIBUTES(obj);

			php_wddx_add_chunk_static(packet, WDDX_STRUCT_S);
			snprintf(tmp_buf, WDDX_BUF_LEN, WDDX_VAR_S, PHP_CLASS_NAME_VAR);
			php_wddx_add_chunk(packet, tmp_buf);
			php_wddx_add_chunk_static(packet, WDDX_STRING_S);
			php_wddx_add_chunk_ex(packet, ZSTR_VAL(class_name), ZSTR_LEN(class_name));
			php_wddx_add_chunk_static(packet, WDDX_STRING_E);
			php_wddx_add_chunk_static(packet, WDDX_VAR_E);

			PHP_CLEANUP_CLASS_ATTRIBUTES();

			objhash = Z_OBJPROP_P(obj);

			ZEND_HASH_FOREACH_VAL(sleephash, varname) {
				if (Z_TYPE_P(varname) != IS_STRING) {
					php_error_docref(NULL, E_NOTICE, "__sleep should return an array only containing the names of instance-variables to serialize.");
					continue;
				}

				if ((ent = zend_hash_find(objhash, Z_STR_P(varname))) != NULL) {
					php_wddx_serialize_var(packet, ent, Z_STR_P(varname));
				}
			} ZEND_HASH_FOREACH_END();

			php_wddx_add_chunk_static(packet, WDDX_STRUCT_E);
		}
	} else {
		PHP_CLASS_ATTRIBUTES;

		PHP_SET_CLASS_ATTRIBUTES(obj);

		php_wddx_add_chunk_static(packet, WDDX_STRUCT_S);
		snprintf(tmp_buf, WDDX_BUF_LEN, WDDX_VAR_S, PHP_CLASS_NAME_VAR);
		php_wddx_add_chunk(packet, tmp_buf);
		php_wddx_add_chunk_static(packet, WDDX_STRING_S);
		php_wddx_add_chunk_ex(packet, ZSTR_VAL(class_name), ZSTR_LEN(class_name));
		php_wddx_add_chunk_static(packet, WDDX_STRING_E);
		php_wddx_add_chunk_static(packet, WDDX_VAR_E);

		PHP_CLEANUP_CLASS_ATTRIBUTES();

		objhash = Z_OBJPROP_P(obj);
		ZEND_HASH_FOREACH_KEY_VAL(objhash, idx, key, ent) {
			if (ent == obj) {
				continue;
			}
			if (key) {
				const char *class_name, *prop_name;
				size_t prop_name_len;
				zend_string *tmp;

				zend_unmangle_property_name_ex(key, &class_name, &prop_name, &prop_name_len);
				tmp = zend_string_init(prop_name, prop_name_len, 0);
				php_wddx_serialize_var(packet, ent, tmp);
				zend_string_release(tmp);
			} else {
				key = zend_long_to_str(idx);
				php_wddx_serialize_var(packet, ent, key);
				zend_string_release(key);
			}
		} ZEND_HASH_FOREACH_END();
		php_wddx_add_chunk_static(packet, WDDX_STRUCT_E);
	}

	zval_ptr_dtor(&fname);
	zval_ptr_dtor(&retval);