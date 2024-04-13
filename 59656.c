ZEND_API int ZEND_FASTCALL zend_hash_move_backwards_ex(HashTable *ht, HashPosition *pos)
{
	uint32_t idx = *pos;

	IS_CONSISTENT(ht);
	HT_ASSERT(&ht->nInternalPointer != pos || GC_REFCOUNT(ht) == 1);

	if (idx != HT_INVALID_IDX) {
		while (idx > 0) {
			idx--;
			if (Z_TYPE(ht->arData[idx].val) != IS_UNDEF) {
				*pos = idx;
				return SUCCESS;
			}
		}
		*pos = HT_INVALID_IDX;
 		return SUCCESS;
	} else {
 		return FAILURE;
	}
}
