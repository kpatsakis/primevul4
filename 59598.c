ZEND_API zval* ZEND_FASTCALL _zend_hash_index_update(HashTable *ht, zend_ulong h, zval *pData ZEND_FILE_LINE_DC)
{
	return _zend_hash_index_add_or_update_i(ht, h, pData, HASH_UPDATE ZEND_FILE_LINE_RELAY_CC);
}
