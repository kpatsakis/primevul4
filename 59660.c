static void zend_always_inline zend_hash_real_init_ex(HashTable *ht, int packed)
{
	HT_ASSERT(GC_REFCOUNT(ht) == 1);
	ZEND_ASSERT(!((ht)->u.flags & HASH_FLAG_INITIALIZED));
	if (packed) {
		HT_SET_DATA_ADDR(ht, pemalloc(HT_SIZE(ht), (ht)->u.flags & HASH_FLAG_PERSISTENT));
		(ht)->u.flags |= HASH_FLAG_INITIALIZED | HASH_FLAG_PACKED;
		HT_HASH_RESET_PACKED(ht);
	} else {
		(ht)->nTableMask = -(ht)->nTableSize;
		HT_SET_DATA_ADDR(ht, pemalloc(HT_SIZE(ht), (ht)->u.flags & HASH_FLAG_PERSISTENT));
		(ht)->u.flags |= HASH_FLAG_INITIALIZED;
		if (EXPECTED(ht->nTableMask == -8)) {
			Bucket *arData = ht->arData;

			HT_HASH_EX(arData, -8) = -1;
			HT_HASH_EX(arData, -7) = -1;
			HT_HASH_EX(arData, -6) = -1;
			HT_HASH_EX(arData, -5) = -1;
			HT_HASH_EX(arData, -4) = -1;
			HT_HASH_EX(arData, -3) = -1;
			HT_HASH_EX(arData, -2) = -1;
			HT_HASH_EX(arData, -1) = -1;
		} else {
			HT_HASH_RESET(ht);
		}
	}
}
