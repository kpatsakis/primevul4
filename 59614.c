ZEND_API void ZEND_FASTCALL zend_array_destroy(HashTable *ht)
{
	Bucket *p, *end;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) <= 1);

	/* break possible cycles */
	GC_REMOVE_FROM_BUFFER(ht);
	GC_TYPE_INFO(ht) = IS_NULL | (GC_WHITE << 16);

	if (ht->nNumUsed) {
		/* In some rare cases destructors of regular arrays may be changed */
		if (UNEXPECTED(ht->pDestructor != ZVAL_PTR_DTOR)) {
			zend_hash_destroy(ht);
			goto free_ht;
		}

		p = ht->arData;
		end = p + ht->nNumUsed;
		SET_INCONSISTENT(HT_IS_DESTROYING);

		if (ht->u.flags & (HASH_FLAG_PACKED|HASH_FLAG_STATIC_KEYS)) {
			do {
				i_zval_ptr_dtor(&p->val ZEND_FILE_LINE_CC);
			} while (++p != end);
		} else if (ht->nNumUsed == ht->nNumOfElements) {
			do {
				i_zval_ptr_dtor(&p->val ZEND_FILE_LINE_CC);
				if (EXPECTED(p->key)) {
					zend_string_release(p->key);
				}
			} while (++p != end);
		} else {
			do {
				if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
					i_zval_ptr_dtor(&p->val ZEND_FILE_LINE_CC);
					if (EXPECTED(p->key)) {
						zend_string_release(p->key);
					}
				}
			} while (++p != end);
		}
		zend_hash_iterators_remove(ht);
		SET_INCONSISTENT(HT_DESTROYED);
	} else if (EXPECTED(!(ht->u.flags & HASH_FLAG_INITIALIZED))) {
		goto free_ht;
	}
	efree(HT_GET_DATA_ADDR(ht));
free_ht:
	FREE_HASHTABLE(ht);
}
