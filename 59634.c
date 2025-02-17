ZEND_API void ZEND_FASTCALL zend_hash_extend(HashTable *ht, uint32_t nSize, zend_bool packed)
{
	HT_ASSERT(GC_REFCOUNT(ht) == 1);
	if (nSize == 0) return;
	if (UNEXPECTED(!((ht)->u.flags & HASH_FLAG_INITIALIZED))) {
		if (nSize > ht->nTableSize) {
			ht->nTableSize = zend_hash_check_size(nSize);
		}
		zend_hash_check_init(ht, packed);
	} else {
		if (packed) {
			ZEND_ASSERT(ht->u.flags & HASH_FLAG_PACKED);
			if (nSize > ht->nTableSize) {
				HANDLE_BLOCK_INTERRUPTIONS();
				ht->nTableSize = zend_hash_check_size(nSize);
				HT_SET_DATA_ADDR(ht, perealloc2(HT_GET_DATA_ADDR(ht), HT_SIZE(ht), HT_USED_SIZE(ht), ht->u.flags & HASH_FLAG_PERSISTENT));
				HANDLE_UNBLOCK_INTERRUPTIONS();
			}
		} else {
			ZEND_ASSERT(!(ht->u.flags & HASH_FLAG_PACKED));
			if (nSize > ht->nTableSize) {
				void *new_data, *old_data = HT_GET_DATA_ADDR(ht);
				Bucket *old_buckets = ht->arData;
				nSize = zend_hash_check_size(nSize);
				HANDLE_BLOCK_INTERRUPTIONS();
				new_data = pemalloc(HT_SIZE_EX(nSize, -nSize), ht->u.flags & HASH_FLAG_PERSISTENT);
				ht->nTableSize = nSize;
				ht->nTableMask = -ht->nTableSize;
				HT_SET_DATA_ADDR(ht, new_data);
				memcpy(ht->arData, old_buckets, sizeof(Bucket) * ht->nNumUsed);
				pefree(old_data, ht->u.flags & HASH_FLAG_PERSISTENT);
				zend_hash_rehash(ht);
				HANDLE_UNBLOCK_INTERRUPTIONS();
			}
		}
	}
}
