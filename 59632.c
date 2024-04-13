static void ZEND_FASTCALL zend_hash_do_resize(HashTable *ht)
{

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	if (ht->nNumUsed > ht->nNumOfElements + (ht->nNumOfElements >> 5)) { /* additional term is there to amortize the cost of compaction */
		HANDLE_BLOCK_INTERRUPTIONS();
		zend_hash_rehash(ht);
		HANDLE_UNBLOCK_INTERRUPTIONS();
	} else if (ht->nTableSize < HT_MAX_SIZE) {	/* Let's double the table size */
		void *new_data, *old_data = HT_GET_DATA_ADDR(ht);
		uint32_t nSize = ht->nTableSize + ht->nTableSize;
		Bucket *old_buckets = ht->arData;

		HANDLE_BLOCK_INTERRUPTIONS();
		new_data = pemalloc(HT_SIZE_EX(nSize, -nSize), ht->u.flags & HASH_FLAG_PERSISTENT);
		ht->nTableSize = nSize;
		ht->nTableMask = -ht->nTableSize;
		HT_SET_DATA_ADDR(ht, new_data);
		memcpy(ht->arData, old_buckets, sizeof(Bucket) * ht->nNumUsed);
		pefree(old_data, ht->u.flags & HASH_FLAG_PERSISTENT);
		zend_hash_rehash(ht);
		HANDLE_UNBLOCK_INTERRUPTIONS();
	} else {
		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%zu * %zu + %zu)", ht->nTableSize * 2, sizeof(Bucket) + sizeof(uint32_t), sizeof(Bucket));
	}
}
