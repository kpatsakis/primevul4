ZEND_API int ZEND_FASTCALL zend_hash_str_del(HashTable *ht, const char *str, size_t len)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	h = zend_inline_hash_func(str, len);
	nIndex = h | ht->nTableMask;

	idx = HT_HASH(ht, nIndex);
	while (idx != HT_INVALID_IDX) {
		p = HT_HASH_TO_BUCKET(ht, idx);
		if ((p->h == h)
			 && p->key
			 && (ZSTR_LEN(p->key) == len)
			 && !memcmp(ZSTR_VAL(p->key), str, len)) {
			_zend_hash_del_el_ex(ht, idx, p, prev);
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}
