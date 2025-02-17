static void zend_always_inline zend_hash_check_init(HashTable *ht, int packed)
{
	HT_ASSERT(GC_REFCOUNT(ht) == 1);
	if (UNEXPECTED(!((ht)->u.flags & HASH_FLAG_INITIALIZED))) {
		zend_hash_real_init_ex(ht, packed);
	}
}
