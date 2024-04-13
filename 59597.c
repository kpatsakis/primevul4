static zend_always_inline zval *_zend_hash_index_add_or_update_i(HashTable *ht, zend_ulong h, zval *pData, uint32_t flag ZEND_FILE_LINE_DC)
{
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	if (UNEXPECTED(!(ht->u.flags & HASH_FLAG_INITIALIZED))) {
		CHECK_INIT(ht, h < ht->nTableSize);
		if (h < ht->nTableSize) {
			p = ht->arData + h;
			goto add_to_packed;
		}
		goto add_to_hash;
	} else if (ht->u.flags & HASH_FLAG_PACKED) {
		if (h < ht->nNumUsed) {
			p = ht->arData + h;
			if (Z_TYPE(p->val) != IS_UNDEF) {
				if (flag & HASH_ADD) {
					return NULL;
				}
				if (ht->pDestructor) {
					ht->pDestructor(&p->val);
				}
				ZVAL_COPY_VALUE(&p->val, pData);
				if ((zend_long)h >= (zend_long)ht->nNextFreeElement) {
					ht->nNextFreeElement = h < ZEND_LONG_MAX ? h + 1 : ZEND_LONG_MAX;
				}
				return &p->val;
			} else { /* we have to keep the order :( */
				goto convert_to_hash;
			}
		} else if (EXPECTED(h < ht->nTableSize)) {
			p = ht->arData + h;
		} else if ((h >> 1) < ht->nTableSize &&
		           (ht->nTableSize >> 1) < ht->nNumOfElements) {
			zend_hash_packed_grow(ht);
			p = ht->arData + h;
		} else {
			goto convert_to_hash;
		}

add_to_packed:
		HANDLE_BLOCK_INTERRUPTIONS();
		/* incremental initialization of empty Buckets */
		if ((flag & (HASH_ADD_NEW|HASH_ADD_NEXT)) == (HASH_ADD_NEW|HASH_ADD_NEXT)) {
			ht->nNumUsed = h + 1;
		} else if (h >= ht->nNumUsed) {
			if (h > ht->nNumUsed) {
				Bucket *q = ht->arData + ht->nNumUsed;
				while (q != p) {
					ZVAL_UNDEF(&q->val);
					q++;
				}
			}
			ht->nNumUsed = h + 1;
		}
		ht->nNumOfElements++;
		if (ht->nInternalPointer == HT_INVALID_IDX) {
			ht->nInternalPointer = h;
		}
		zend_hash_iterators_update(ht, HT_INVALID_IDX, h);
		if ((zend_long)h >= (zend_long)ht->nNextFreeElement) {
			ht->nNextFreeElement = h < ZEND_LONG_MAX ? h + 1 : ZEND_LONG_MAX;
		}
		p->h = h;
		p->key = NULL;
		ZVAL_COPY_VALUE(&p->val, pData);

		HANDLE_UNBLOCK_INTERRUPTIONS();

		return &p->val;

convert_to_hash:
		zend_hash_packed_to_hash(ht);
	} else if ((flag & HASH_ADD_NEW) == 0) {
		p = zend_hash_index_find_bucket(ht, h);
		if (p) {
			if (flag & HASH_ADD) {
				return NULL;
			}
			ZEND_ASSERT(&p->val != pData);
			HANDLE_BLOCK_INTERRUPTIONS();
			if (ht->pDestructor) {
				ht->pDestructor(&p->val);
			}
			ZVAL_COPY_VALUE(&p->val, pData);
			HANDLE_UNBLOCK_INTERRUPTIONS();
			if ((zend_long)h >= (zend_long)ht->nNextFreeElement) {
				ht->nNextFreeElement = h < ZEND_LONG_MAX ? h + 1 : ZEND_LONG_MAX;
			}
			return &p->val;
		}
	}

	ZEND_HASH_IF_FULL_DO_RESIZE(ht);		/* If the Hash table is full, resize it */

add_to_hash:
	HANDLE_BLOCK_INTERRUPTIONS();
	idx = ht->nNumUsed++;
	ht->nNumOfElements++;
	if (ht->nInternalPointer == HT_INVALID_IDX) {
		ht->nInternalPointer = idx;
	}
	zend_hash_iterators_update(ht, HT_INVALID_IDX, idx);
	if ((zend_long)h >= (zend_long)ht->nNextFreeElement) {
		ht->nNextFreeElement = h < ZEND_LONG_MAX ? h + 1 : ZEND_LONG_MAX;
	}
	p = ht->arData + idx;
	p->h = h;
	p->key = NULL;
	nIndex = h | ht->nTableMask;
	ZVAL_COPY_VALUE(&p->val, pData);
	Z_NEXT(p->val) = HT_HASH(ht, nIndex);
	HT_HASH(ht, nIndex) = HT_IDX_TO_HASH(idx);
	HANDLE_UNBLOCK_INTERRUPTIONS();

	return &p->val;
}
