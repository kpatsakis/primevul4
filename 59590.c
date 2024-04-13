ZEND_API zval* ZEND_FASTCALL _zend_hash_add_or_update(HashTable *ht, zend_string *key, zval *pData, uint32_t flag ZEND_FILE_LINE_DC)
{
	return _zend_hash_add_or_update_i(ht, key, pData, flag ZEND_FILE_LINE_RELAY_CC);
}
