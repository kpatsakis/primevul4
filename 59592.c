static zend_always_inline void _zend_hash_del_el(HashTable *ht, uint32_t idx, Bucket *p)
{
	Bucket *prev = NULL;

	if (!(ht->u.flags & HASH_FLAG_PACKED)) {
		uint32_t nIndex = p->h | ht->nTableMask;
		uint32_t i = HT_HASH(ht, nIndex);

		if (i != idx) {
			prev = HT_HASH_TO_BUCKET(ht, i);
			while (Z_NEXT(prev->val) != idx) {
				i = Z_NEXT(prev->val);
				prev = HT_HASH_TO_BUCKET(ht, i);
			}
	 	}
	}

	_zend_hash_del_el_ex(ht, idx, p, prev);
}
