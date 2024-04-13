static zend_always_inline void _zend_hash_del_el_ex(HashTable *ht, uint32_t idx, Bucket *p, Bucket *prev)
{
	HANDLE_BLOCK_INTERRUPTIONS();
	if (!(ht->u.flags & HASH_FLAG_PACKED)) {
		if (prev) {
			Z_NEXT(prev->val) = Z_NEXT(p->val);
		} else {
			HT_HASH(ht, p->h | ht->nTableMask) = Z_NEXT(p->val);
		}
	}
	if (HT_IDX_TO_HASH(ht->nNumUsed - 1) == idx) {
		do {
			ht->nNumUsed--;
		} while (ht->nNumUsed > 0 && (UNEXPECTED(Z_TYPE(ht->arData[ht->nNumUsed-1].val) == IS_UNDEF)));
	}
	ht->nNumOfElements--;
	if (HT_IDX_TO_HASH(ht->nInternalPointer) == idx || UNEXPECTED(ht->u.v.nIteratorsCount)) {
		uint32_t new_idx;

		new_idx = idx = HT_HASH_TO_IDX(idx);
		while (1) {
			new_idx++;
			if (new_idx >= ht->nNumUsed) {
				new_idx = HT_INVALID_IDX;
				break;
			} else if (Z_TYPE(ht->arData[new_idx].val) != IS_UNDEF) {
				break;
			}
		}
		if (ht->nInternalPointer == idx) {
			ht->nInternalPointer = new_idx;
		}
		zend_hash_iterators_update(ht, idx, new_idx);
	}
	if (p->key) {
		zend_string_release(p->key);
	}
	if (ht->pDestructor) {
		zval tmp;
		ZVAL_COPY_VALUE(&tmp, &p->val);
		ZVAL_UNDEF(&p->val);
		ht->pDestructor(&tmp);
	} else {
		ZVAL_UNDEF(&p->val);
	}
	HANDLE_UNBLOCK_INTERRUPTIONS();
}
