ZEND_API zend_bool ZEND_FASTCALL zend_hash_exists(const HashTable *ht, zend_string *key)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	p = zend_hash_find_bucket(ht, key);
	return p ? 1 : 0;
}
