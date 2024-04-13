 static void ZEND_FASTCALL zend_hash_packed_grow(HashTable *ht)
{
	HT_ASSERT(GC_REFCOUNT(ht) == 1);
	if (ht->nTableSize >= HT_MAX_SIZE) {
		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%zu * %zu + %zu)", ht->nTableSize * 2, sizeof(Bucket), sizeof(Bucket));
	}
	HANDLE_BLOCK_INTERRUPTIONS();
	ht->nTableSize += ht->nTableSize;
	HT_SET_DATA_ADDR(ht, perealloc2(HT_GET_DATA_ADDR(ht), HT_SIZE(ht), HT_USED_SIZE(ht), ht->u.flags & HASH_FLAG_PERSISTENT));
	HANDLE_UNBLOCK_INTERRUPTIONS();
}
