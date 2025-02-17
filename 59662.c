static zend_bool ZEND_FASTCALL zend_hash_replace_checker_wrapper(HashTable *target, zval *source_data, Bucket *p, void *pParam, merge_checker_func_t merge_checker_func)
{
	zend_hash_key hash_key;

	hash_key.h = p->h;
	hash_key.key = p->key;
	return merge_checker_func(target, source_data, &hash_key, pParam);
}
