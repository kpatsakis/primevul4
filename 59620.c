ZEND_API zval* ZEND_FASTCALL zend_hash_add_empty_element(HashTable *ht, zend_string *key)
{
	zval dummy;

	ZVAL_NULL(&dummy);
	return zend_hash_add(ht, key, &dummy);
}
