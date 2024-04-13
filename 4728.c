 */
void php_wddx_serialize_var(wddx_packet *packet, zval *var, zend_string *name)
{
	HashTable *ht;

	if (name) {
		char *tmp_buf;
		zend_string *name_esc = php_escape_html_entities((unsigned char *) ZSTR_VAL(name), ZSTR_LEN(name), 0, ENT_QUOTES, NULL);
		tmp_buf = emalloc(ZSTR_LEN(name_esc) + sizeof(WDDX_VAR_S));
		snprintf(tmp_buf, ZSTR_LEN(name_esc) + sizeof(WDDX_VAR_S), WDDX_VAR_S, ZSTR_VAL(name_esc));
		php_wddx_add_chunk(packet, tmp_buf);
		efree(tmp_buf);
		zend_string_release(name_esc);
	}

	if (Z_TYPE_P(var) == IS_INDIRECT) {
		var = Z_INDIRECT_P(var);
	}
	ZVAL_DEREF(var);
	switch (Z_TYPE_P(var)) {
		case IS_STRING:
			php_wddx_serialize_string(packet, var);
			break;

		case IS_LONG:
		case IS_DOUBLE:
			php_wddx_serialize_number(packet, var);
			break;

		case IS_TRUE:
		case IS_FALSE:
			php_wddx_serialize_boolean(packet, var);
			break;

		case IS_NULL:
			php_wddx_serialize_unset(packet);
			break;

		case IS_ARRAY:
			ht = Z_ARRVAL_P(var);
			if (ht->u.v.nApplyCount > 1) {
				php_error_docref(NULL, E_RECOVERABLE_ERROR, "WDDX doesn't support circular references");
				return;
			}
			if (ZEND_HASH_APPLY_PROTECTION(ht)) {
				ht->u.v.nApplyCount++;
			}
			php_wddx_serialize_array(packet, var);
			if (ZEND_HASH_APPLY_PROTECTION(ht)) {
				ht->u.v.nApplyCount--;
			}
			break;

		case IS_OBJECT:
			ht = Z_OBJPROP_P(var);
			if (ht->u.v.nApplyCount > 1) {
				php_error_docref(NULL, E_RECOVERABLE_ERROR, "WDDX doesn't support circular references");
				return;
			}
			ht->u.v.nApplyCount++;
 			php_wddx_serialize_object(packet, var);
			ht->u.v.nApplyCount--;
			break;
	}

	if (name) {
		php_wddx_add_chunk_static(packet, WDDX_VAR_E);
	}