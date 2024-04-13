 */
static void php_wddx_add_var(wddx_packet *packet, zval *name_var)
{
	zval *val;
	HashTable *target_hash;

	if (Z_TYPE_P(name_var) == IS_STRING) {
		zend_array *symbol_table = zend_rebuild_symbol_table();
		if ((val = zend_hash_find(symbol_table, Z_STR_P(name_var))) != NULL) {
			if (Z_TYPE_P(val) == IS_INDIRECT) {
				val = Z_INDIRECT_P(val);
			}
			php_wddx_serialize_var(packet, val, Z_STR_P(name_var));
		}
	} else if (Z_TYPE_P(name_var) == IS_ARRAY || Z_TYPE_P(name_var) == IS_OBJECT)	{
		int is_array = Z_TYPE_P(name_var) == IS_ARRAY;

		target_hash = HASH_OF(name_var);

		if (is_array && target_hash->u.v.nApplyCount > 1) {
			php_error_docref(NULL, E_WARNING, "recursion detected");
			return;
		}

		if (Z_IMMUTABLE_P(name_var)) {
			ZEND_HASH_FOREACH_VAL(target_hash, val) {
				php_wddx_add_var(packet, val);
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEND_HASH_FOREACH_VAL(target_hash, val) {
				if (is_array) {
					target_hash->u.v.nApplyCount++;
				}

				ZVAL_DEREF(val);
				php_wddx_add_var(packet, val);

				if (is_array) {
					target_hash->u.v.nApplyCount--;
				}
			} ZEND_HASH_FOREACH_END();
		}
	}