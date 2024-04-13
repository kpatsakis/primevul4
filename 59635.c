ZEND_API zval* ZEND_FASTCALL zend_hash_find(const HashTable *ht, zend_string *key)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	p = zend_hash_find_bucket(ht, key);
	return p ? &p->val : NULL;
}
