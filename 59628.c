static zend_always_inline int zend_hash_compare_impl(HashTable *ht1, HashTable *ht2, compare_func_t compar, zend_bool ordered) {
	uint32_t idx1, idx2;

	if (ht1->nNumOfElements != ht2->nNumOfElements) {
		return ht1->nNumOfElements > ht2->nNumOfElements ? 1 : -1;
	}

	for (idx1 = 0, idx2 = 0; idx1 < ht1->nNumUsed; idx1++) {
		Bucket *p1 = ht1->arData + idx1, *p2;
		zval *pData1, *pData2;
		int result;

		if (Z_TYPE(p1->val) == IS_UNDEF) continue;
		if (ordered) {
			while (1) {
				ZEND_ASSERT(idx2 != ht2->nNumUsed);
				p2 = ht2->arData + idx2;
				if (Z_TYPE(p2->val) != IS_UNDEF) break;
				idx2++;
			}
			if (p1->key == NULL && p2->key == NULL) { /* numeric indices */
				if (p1->h != p2->h) {
					return p1->h > p2->h ? 1 : -1;
				}
			} else if (p1->key != NULL && p2->key != NULL) { /* string indices */
				if (ZSTR_LEN(p1->key) != ZSTR_LEN(p2->key)) {
					return ZSTR_LEN(p1->key) > ZSTR_LEN(p2->key) ? 1 : -1;
				}

				result = memcmp(ZSTR_VAL(p1->key), ZSTR_VAL(p2->key), ZSTR_LEN(p1->key));
				if (result != 0) {
					return result;
				}
			} else {
				/* Mixed key types: A string key is considered as larger */
				return p1->key != NULL ? 1 : -1;
			}
			pData2 = &p2->val;
			idx2++;
		} else {
			if (p1->key == NULL) { /* numeric index */
				pData2 = zend_hash_index_find(ht2, p1->h);
				if (pData2 == NULL) {
					return 1;
				}
			} else { /* string index */
				pData2 = zend_hash_find(ht2, p1->key);
				if (pData2 == NULL) {
					return 1;
				}
			}
		}

		pData1 = &p1->val;
		if (Z_TYPE_P(pData1) == IS_INDIRECT) {
			pData1 = Z_INDIRECT_P(pData1);
		}
		if (Z_TYPE_P(pData2) == IS_INDIRECT) {
			pData2 = Z_INDIRECT_P(pData2);
		}

		if (Z_TYPE_P(pData1) == IS_UNDEF) {
			if (Z_TYPE_P(pData2) != IS_UNDEF) {
				return -1;
			}
		} else if (Z_TYPE_P(pData2) == IS_UNDEF) {
			return 1;
		} else {
			result = compar(pData1, pData2);
			if (result != 0) {
				return result;
			}
		}
	}

	return 0;
}
