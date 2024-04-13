ZEND_API zval* ZEND_FASTCALL _zend_hash_str_add_or_update(HashTable *ht, const char *str, size_t len, zval *pData, uint32_t flag ZEND_FILE_LINE_DC)
{
	zend_string *key = zend_string_init(str, len, ht->u.flags & HASH_FLAG_PERSISTENT);
	zval *ret = _zend_hash_add_or_update_i(ht, key, pData, flag ZEND_FILE_LINE_RELAY_CC);
	zend_string_release(key);
	return ret;
}
