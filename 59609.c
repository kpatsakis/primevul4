ZEND_API zval* ZEND_FASTCALL _zend_hash_str_update_ind(HashTable *ht, const char *str, size_t len, zval *pData ZEND_FILE_LINE_DC)
{
	zend_string *key = zend_string_init(str, len, ht->u.flags & HASH_FLAG_PERSISTENT);
	zval *ret = _zend_hash_add_or_update_i(ht, key, pData, HASH_UPDATE | HASH_UPDATE_INDIRECT ZEND_FILE_LINE_RELAY_CC);
	zend_string_release(key);
	return ret;
}
