ZEND_API void ZEND_FASTCALL zend_hash_packed_to_hash(HashTable *ht)
{
	void *new_data, *old_data = HT_GET_DATA_ADDR(ht);
	Bucket *old_buckets = ht->arData;

	HT_ASSERT(GC_REFCOUNT(ht) == 1);
	HANDLE_BLOCK_INTERRUPTIONS();
	ht->u.flags &= ~HASH_FLAG_PACKED;
	new_data = pemalloc(HT_SIZE_EX(ht->nTableSize, -ht->nTableSize), (ht)->u.flags & HASH_FLAG_PERSISTENT);
	ht->nTableMask = -ht->nTableSize;
	HT_SET_DATA_ADDR(ht, new_data);
	memcpy(ht->arData, old_buckets, sizeof(Bucket) * ht->nNumUsed);
	pefree(old_data, (ht)->u.flags & HASH_FLAG_PERSISTENT);
	zend_hash_rehash(ht);
	HANDLE_UNBLOCK_INTERRUPTIONS();
}
