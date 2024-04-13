ZEND_API zval* ZEND_FASTCALL _zend_hash_index_add_or_update(HashTable *ht, zend_ulong h, zval *pData, uint32_t flag ZEND_FILE_LINE_DC)
{
	return _zend_hash_index_add_or_update_i(ht, h, pData, flag ZEND_FILE_LINE_RELAY_CC);
}
