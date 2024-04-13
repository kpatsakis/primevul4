ZEND_API zval* ZEND_FASTCALL zend_hash_index_add_empty_element(HashTable *ht, zend_ulong h)
{
	zval dummy;

	ZVAL_NULL(&dummy);
	return zend_hash_index_add(ht, h, &dummy);
}
