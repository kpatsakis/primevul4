ZEND_API void ZEND_FASTCALL zend_hash_internal_pointer_reset_ex(HashTable *ht, HashPosition *pos)
{
    uint32_t idx;

	IS_CONSISTENT(ht);
	HT_ASSERT(&ht->nInternalPointer != pos || GC_REFCOUNT(ht) == 1);

	for (idx = 0; idx < ht->nNumUsed; idx++) {
		if (Z_TYPE(ht->arData[idx].val) != IS_UNDEF) {
			*pos = idx;
			return;
		}
	}
	*pos = HT_INVALID_IDX;
}
