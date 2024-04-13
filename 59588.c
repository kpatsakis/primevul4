ZEND_API zval* ZEND_FASTCALL _zend_hash_add(HashTable *ht, zend_string *key, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_add_or_update_i(ht, key, pData, HASH_ADD ZEND_FILE_LINE_RELAY_CC);
}
