ZEND_API int zend_hash_compare(HashTable *ht1, HashTable *ht2, compare_func_t compar, zend_bool ordered)
{
	int result;
	IS_CONSISTENT(ht1);
	IS_CONSISTENT(ht2);

	HASH_PROTECT_RECURSION(ht1);
	HASH_PROTECT_RECURSION(ht2);
	result = zend_hash_compare_impl(ht1, ht2, compar, ordered);
	HASH_UNPROTECT_RECURSION(ht1);
	HASH_UNPROTECT_RECURSION(ht2);

	return result;
}
