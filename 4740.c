 */
static void php_wddx_serialize_array(wddx_packet *packet, zval *arr)
{
	zval *ent;
	zend_string *key;
	int is_struct = 0;
	zend_ulong idx;
	HashTable *target_hash;
	char tmp_buf[WDDX_BUF_LEN];
	zend_ulong ind = 0;

	target_hash = Z_ARRVAL_P(arr);
	ZEND_HASH_FOREACH_KEY(target_hash, idx, key) {
		if (key) {
			is_struct = 1;
			break;
		}

		if (idx != ind) {
			is_struct = 1;
			break;
		}
		ind++;
	} ZEND_HASH_FOREACH_END();

	if (is_struct) {
		php_wddx_add_chunk_static(packet, WDDX_STRUCT_S);
	} else {
		snprintf(tmp_buf, sizeof(tmp_buf), WDDX_ARRAY_S, zend_hash_num_elements(target_hash));
		php_wddx_add_chunk(packet, tmp_buf);
	}

	ZEND_HASH_FOREACH_KEY_VAL(target_hash, idx, key, ent) {
		if (ent == arr) {
			continue;
		}

		if (is_struct) {
			if (key) {
				php_wddx_serialize_var(packet, ent, key);
			} else {
				key = zend_long_to_str(idx);
				php_wddx_serialize_var(packet, ent, key);
				zend_string_release(key);
			}
		} else {
			php_wddx_serialize_var(packet, ent, NULL);
		}
	} ZEND_HASH_FOREACH_END();

	if (is_struct) {
		php_wddx_add_chunk_static(packet, WDDX_STRUCT_E);
	} else {
		php_wddx_add_chunk_static(packet, WDDX_ARRAY_E);
	}