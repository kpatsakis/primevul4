ZEND_API void ZEND_FASTCALL zend_hash_graceful_reverse_destroy(HashTable *ht)
{
	uint32_t idx;
	Bucket *p;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	idx = ht->nNumUsed;
	p = ht->arData + ht->nNumUsed;
	while (idx > 0) {
		idx--;
		p--;
		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
		_zend_hash_del_el(ht, HT_IDX_TO_HASH(idx), p);
	}

	if (ht->u.flags & HASH_FLAG_INITIALIZED) {
		pefree(HT_GET_DATA_ADDR(ht), ht->u.flags & HASH_FLAG_PERSISTENT);
	}

	SET_INCONSISTENT(HT_DESTROYED);
}
