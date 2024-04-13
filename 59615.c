ZEND_API HashTable* ZEND_FASTCALL zend_array_dup(HashTable *source)
{
	uint32_t idx;
	HashTable *target;

	IS_CONSISTENT(source);

	ALLOC_HASHTABLE(target);
	GC_REFCOUNT(target) = 1;
	GC_TYPE_INFO(target) = IS_ARRAY;

	target->nTableSize = source->nTableSize;
	target->pDestructor = source->pDestructor;

	if (source->nNumUsed == 0) {
		target->u.flags = (source->u.flags & ~(HASH_FLAG_INITIALIZED|HASH_FLAG_PACKED|HASH_FLAG_PERSISTENT|ZEND_HASH_APPLY_COUNT_MASK)) | HASH_FLAG_APPLY_PROTECTION | HASH_FLAG_STATIC_KEYS;
		target->nTableMask = HT_MIN_MASK;
		target->nNumUsed = 0;
		target->nNumOfElements = 0;
		target->nNextFreeElement = 0;
		target->nInternalPointer = HT_INVALID_IDX;
		HT_SET_DATA_ADDR(target, &uninitialized_bucket);
	} else if (GC_FLAGS(source) & IS_ARRAY_IMMUTABLE) {
		target->u.flags = (source->u.flags & ~HASH_FLAG_PERSISTENT) | HASH_FLAG_APPLY_PROTECTION;
		target->nTableMask = source->nTableMask;
		target->nNumUsed = source->nNumUsed;
		target->nNumOfElements = source->nNumOfElements;
		target->nNextFreeElement = source->nNextFreeElement;
		HT_SET_DATA_ADDR(target, emalloc(HT_SIZE(target)));
		target->nInternalPointer = source->nInternalPointer;
		memcpy(HT_GET_DATA_ADDR(target), HT_GET_DATA_ADDR(source), HT_USED_SIZE(source));
		if (target->nNumOfElements > 0 &&
		    target->nInternalPointer == HT_INVALID_IDX) {
			idx = 0;
			while (Z_TYPE(target->arData[idx].val) == IS_UNDEF) {
				idx++;
			}
			target->nInternalPointer = idx;
		}
	} else if (source->u.flags & HASH_FLAG_PACKED) {
		target->u.flags = (source->u.flags & ~(HASH_FLAG_PERSISTENT|ZEND_HASH_APPLY_COUNT_MASK)) | HASH_FLAG_APPLY_PROTECTION;
		target->nTableMask = source->nTableMask;
		target->nNumUsed = source->nNumUsed;
		target->nNumOfElements = source->nNumOfElements;
		target->nNextFreeElement = source->nNextFreeElement;
		HT_SET_DATA_ADDR(target, emalloc(HT_SIZE(target)));
		target->nInternalPointer = source->nInternalPointer;
		HT_HASH_RESET_PACKED(target);

		if (target->nNumUsed == target->nNumOfElements) {
			zend_array_dup_packed_elements(source, target, 0);
		} else {
			zend_array_dup_packed_elements(source, target, 1);
		}
		if (target->nNumOfElements > 0 &&
		    target->nInternalPointer == HT_INVALID_IDX) {
			idx = 0;
			while (Z_TYPE(target->arData[idx].val) == IS_UNDEF) {
				idx++;
			}
			target->nInternalPointer = idx;
		}
	} else {
		target->u.flags = (source->u.flags & ~(HASH_FLAG_PERSISTENT|ZEND_HASH_APPLY_COUNT_MASK)) | HASH_FLAG_APPLY_PROTECTION;
		target->nTableMask = source->nTableMask;
		target->nNextFreeElement = source->nNextFreeElement;
		target->nInternalPointer = source->nInternalPointer;

		HT_SET_DATA_ADDR(target, emalloc(HT_SIZE(target)));
		HT_HASH_RESET(target);

		if (target->u.flags & HASH_FLAG_STATIC_KEYS) {
			if (source->nNumUsed == source->nNumOfElements) {
				idx = zend_array_dup_elements(source, target, 1, 0);
			} else {
				idx = zend_array_dup_elements(source, target, 1, 1);
			}
		} else {
			if (source->nNumUsed == source->nNumOfElements) {
				idx = zend_array_dup_elements(source, target, 0, 0);
			} else {
				idx = zend_array_dup_elements(source, target, 0, 1);
			}
		}
		target->nNumUsed = idx;
		target->nNumOfElements = idx;
		if (idx > 0 && target->nInternalPointer == HT_INVALID_IDX) {
			target->nInternalPointer = 0;
		}
	}
	return target;
}
