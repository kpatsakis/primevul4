ZEND_API zval* ZEND_FASTCALL zend_hash_str_add_empty_element(HashTable *ht, const char *str, size_t len)
{
	zval dummy;

	ZVAL_NULL(&dummy);
	return zend_hash_str_add(ht, str, len, &dummy);
}
