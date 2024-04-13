ZEND_API void ZEND_FASTCALL zend_hash_real_init(HashTable *ht, zend_bool packed)
{
	IS_CONSISTENT(ht);

	HT_ASSERT(GC_REFCOUNT(ht) == 1);
	zend_hash_real_init_ex(ht, packed);
}
