ZEND_API zval* ZEND_FASTCALL zend_hash_minmax(const HashTable *ht, compare_func_t compar, uint32_t flag)
{
	uint32_t idx;
	Bucket *p, *res;

	IS_CONSISTENT(ht);

	if (ht->nNumOfElements == 0 ) {
		return NULL;
	}

	idx = 0;
	while (1) {
		if (idx == ht->nNumUsed) {
			return NULL;
		}
		if (Z_TYPE(ht->arData[idx].val) != IS_UNDEF) break;
		idx++;
	}
	res = ht->arData + idx;
	for (; idx < ht->nNumUsed; idx++) {
		p = ht->arData + idx;
		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;

		if (flag) {
			if (compar(res, p) < 0) { /* max */
				res = p;
			}
		} else {
			if (compar(res, p) > 0) { /* min */
				res = p;
			}
		}
	}
	return &res->val;
}
