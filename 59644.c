ZEND_API zend_bool ZEND_FASTCALL zend_hash_index_exists(const HashTable *ht, zend_ulong h)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	if (ht->u.flags & HASH_FLAG_PACKED) {
		if (h < ht->nNumUsed) {
			if (Z_TYPE(ht->arData[h].val) != IS_UNDEF) {
				return 1;
			}
		}
		return 0;
	}

	p = zend_hash_index_find_bucket(ht, h);
	return p ? 1 : 0;
}
