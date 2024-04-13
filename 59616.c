static zend_always_inline int zend_array_dup_element(HashTable *source, HashTable *target, uint32_t idx, Bucket *p, Bucket *q, int packed, int static_keys, int with_holes)
{
	zval *data = &p->val;

	if (with_holes) {
		if (!packed && Z_TYPE_INFO_P(data) == IS_INDIRECT) {
			data = Z_INDIRECT_P(data);
		}
		if (UNEXPECTED(Z_TYPE_INFO_P(data) == IS_UNDEF)) {
			return 0;
		}
	} else if (!packed) {
		/* INDIRECT element may point to UNDEF-ined slots */
		if (Z_TYPE_INFO_P(data) == IS_INDIRECT) {
			data = Z_INDIRECT_P(data);
			if (UNEXPECTED(Z_TYPE_INFO_P(data) == IS_UNDEF)) {
				return 0;
			}
		}
	}

	do {
		if (Z_OPT_REFCOUNTED_P(data)) {
			if (Z_ISREF_P(data) && Z_REFCOUNT_P(data) == 1 &&
			    (Z_TYPE_P(Z_REFVAL_P(data)) != IS_ARRAY ||
			      Z_ARRVAL_P(Z_REFVAL_P(data)) != source)) {
				data = Z_REFVAL_P(data);
				if (!Z_OPT_REFCOUNTED_P(data)) {
					break;
				}
			}
			Z_ADDREF_P(data);
		}
	} while (0);
	ZVAL_COPY_VALUE(&q->val, data);

	q->h = p->h;
	if (packed) {
		q->key = NULL;
	} else {
		uint32_t nIndex;

		q->key = p->key;
		if (!static_keys && q->key) {
			zend_string_addref(q->key);
		}

		nIndex = q->h | target->nTableMask;
		Z_NEXT(q->val) = HT_HASH(target, nIndex);
		HT_HASH(target, nIndex) = HT_IDX_TO_HASH(idx);
	}
	return 1;
}
