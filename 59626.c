ZEND_API void ZEND_FASTCALL zend_hash_clean(HashTable *ht)
{
	Bucket *p, *end;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	if (ht->nNumUsed) {
		p = ht->arData;
		end = p + ht->nNumUsed;
		if (ht->pDestructor) {
			if (ht->u.flags & (HASH_FLAG_PACKED|HASH_FLAG_STATIC_KEYS)) {
				if (ht->nNumUsed == ht->nNumOfElements) {
					do {
						ht->pDestructor(&p->val);
					} while (++p != end);
				} else {
					do {
						if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
							ht->pDestructor(&p->val);
						}
					} while (++p != end);
				}
			} else if (ht->nNumUsed == ht->nNumOfElements) {
				do {
					ht->pDestructor(&p->val);
					if (EXPECTED(p->key)) {
						zend_string_release(p->key);
					}
				} while (++p != end);
			} else {
				do {
					if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
						ht->pDestructor(&p->val);
						if (EXPECTED(p->key)) {
							zend_string_release(p->key);
						}
					}
				} while (++p != end);
			}
		} else {
			if (!(ht->u.flags & (HASH_FLAG_PACKED|HASH_FLAG_STATIC_KEYS))) {
				if (ht->nNumUsed == ht->nNumOfElements) {
					do {
						if (EXPECTED(p->key)) {
							zend_string_release(p->key);
						}
					} while (++p != end);
				} else {
					do {
						if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
							if (EXPECTED(p->key)) {
								zend_string_release(p->key);
							}
						}
					} while (++p != end);
				}
			}
		}
		if (!(ht->u.flags & HASH_FLAG_PACKED)) {
			HT_HASH_RESET(ht);
		}
	}
	ht->nNumUsed = 0;
	ht->nNumOfElements = 0;
	ht->nNextFreeElement = 0;
	ht->nInternalPointer = HT_INVALID_IDX;
}
