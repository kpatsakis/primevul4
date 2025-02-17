ZEND_API void ZEND_FASTCALL zend_hash_to_packed(HashTable *ht)
{
	void *new_data, *old_data = HT_GET_DATA_ADDR(ht);
	Bucket *old_buckets = ht->arData;

	HT_ASSERT(GC_REFCOUNT(ht) == 1);
	HANDLE_BLOCK_INTERRUPTIONS();
	new_data = pemalloc(HT_SIZE_EX(ht->nTableSize, HT_MIN_MASK), (ht)->u.flags & HASH_FLAG_PERSISTENT);
	ht->u.flags |= HASH_FLAG_PACKED | HASH_FLAG_STATIC_KEYS;
	ht->nTableMask = HT_MIN_MASK;
	HT_SET_DATA_ADDR(ht, new_data);
	HT_HASH_RESET_PACKED(ht);
	memcpy(ht->arData, old_buckets, sizeof(Bucket) * ht->nNumUsed);
	pefree(old_data, (ht)->u.flags & HASH_FLAG_PERSISTENT);
	HANDLE_UNBLOCK_INTERRUPTIONS();
}
