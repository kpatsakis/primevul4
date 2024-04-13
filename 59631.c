ZEND_API int ZEND_FASTCALL zend_hash_del_ind(HashTable *ht, zend_string *key)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	h = zend_string_hash_val(key);
	nIndex = h | ht->nTableMask;

	idx = HT_HASH(ht, nIndex);
	while (idx != HT_INVALID_IDX) {
		p = HT_HASH_TO_BUCKET(ht, idx);
		if ((p->key == key) ||
			(p->h == h &&
		     p->key &&
		     ZSTR_LEN(p->key) == ZSTR_LEN(key) &&
		     memcmp(ZSTR_VAL(p->key), ZSTR_VAL(key), ZSTR_LEN(key)) == 0)) {
			if (Z_TYPE(p->val) == IS_INDIRECT) {
				zval *data = Z_INDIRECT(p->val);

				if (UNEXPECTED(Z_TYPE_P(data) == IS_UNDEF)) {
					return FAILURE;
				} else {
					if (ht->pDestructor) {
						zval tmp;
						ZVAL_COPY_VALUE(&tmp, data);
						ZVAL_UNDEF(data);
						ht->pDestructor(&tmp);
					} else {
						ZVAL_UNDEF(data);
					}
					ht->u.v.flags |= HASH_FLAG_HAS_EMPTY_IND;
				}
			} else {
				_zend_hash_del_el_ex(ht, idx, p, prev);
			}
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}
