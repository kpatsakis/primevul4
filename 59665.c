ZEND_API int ZEND_FASTCALL zend_hash_sort_ex(HashTable *ht, sort_func_t sort, compare_func_t compar, zend_bool renumber)
{
	Bucket *p;
	uint32_t i, j;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	if (!(ht->nNumOfElements>1) && !(renumber && ht->nNumOfElements>0)) { /* Doesn't require sorting */
		return SUCCESS;
	}

	if (ht->nNumUsed == ht->nNumOfElements) {
		i = ht->nNumUsed;
	} else {
		for (j = 0, i = 0; j < ht->nNumUsed; j++) {
			p = ht->arData + j;
			if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
			if (i != j) {
				ht->arData[i] = *p;
			}
			i++;
		}
	}

	sort((void *)ht->arData, i, sizeof(Bucket), compar,
			(swap_func_t)(renumber? zend_hash_bucket_renum_swap :
				((ht->u.flags & HASH_FLAG_PACKED) ? zend_hash_bucket_packed_swap : zend_hash_bucket_swap)));

	HANDLE_BLOCK_INTERRUPTIONS();
	ht->nNumUsed = i;
	ht->nInternalPointer = 0;

	if (renumber) {
		for (j = 0; j < i; j++) {
			p = ht->arData + j;
			p->h = j;
			if (p->key) {
				zend_string_release(p->key);
				p->key = NULL;
			}
		}

		ht->nNextFreeElement = i;
	}
	if (ht->u.flags & HASH_FLAG_PACKED) {
		if (!renumber) {
			zend_hash_packed_to_hash(ht);
		}
	} else {
		if (renumber) {
			void *new_data, *old_data = HT_GET_DATA_ADDR(ht);
			Bucket *old_buckets = ht->arData;

			new_data = pemalloc(HT_SIZE_EX(ht->nTableSize, HT_MIN_MASK), (ht->u.flags & HASH_FLAG_PERSISTENT));
			ht->u.flags |= HASH_FLAG_PACKED | HASH_FLAG_STATIC_KEYS;
			ht->nTableMask = HT_MIN_MASK;
			HT_SET_DATA_ADDR(ht, new_data);
			memcpy(ht->arData, old_buckets, sizeof(Bucket) * ht->nNumUsed);
			pefree(old_data, ht->u.flags & HASH_FLAG_PERSISTENT & HASH_FLAG_PERSISTENT);
			HT_HASH_RESET_PACKED(ht);
		} else {
			zend_hash_rehash(ht);
		}
	}

	HANDLE_UNBLOCK_INTERRUPTIONS();

	return SUCCESS;
}
