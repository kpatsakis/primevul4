ZEND_API void ZEND_FASTCALL zend_hash_copy(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor)
{
    uint32_t idx;
	Bucket *p;
	zval *new_entry, *data;
	zend_bool setTargetPointer;

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);
	HT_ASSERT(GC_REFCOUNT(target) == 1);

	setTargetPointer = (target->nInternalPointer == HT_INVALID_IDX);
	for (idx = 0; idx < source->nNumUsed; idx++) {
		p = source->arData + idx;
		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;

		if (setTargetPointer && source->nInternalPointer == idx) {
			target->nInternalPointer = HT_INVALID_IDX;
		}
		/* INDIRECT element may point to UNDEF-ined slots */
		data = &p->val;
		if (Z_TYPE_P(data) == IS_INDIRECT) {
			data = Z_INDIRECT_P(data);
			if (UNEXPECTED(Z_TYPE_P(data) == IS_UNDEF)) {
				continue;
			}
		}
		if (p->key) {
			new_entry = zend_hash_update(target, p->key, data);
		} else {
			new_entry = zend_hash_index_update(target, p->h, data);
		}
		if (pCopyConstructor) {
			pCopyConstructor(new_entry);
		}
	}
	if (target->nInternalPointer == HT_INVALID_IDX && target->nNumOfElements > 0) {
		idx = 0;
		while (Z_TYPE(target->arData[idx].val) == IS_UNDEF) {
			idx++;
		}
		target->nInternalPointer = idx;
	}
}
