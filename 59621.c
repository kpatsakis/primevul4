ZEND_API void ZEND_FASTCALL zend_hash_apply(HashTable *ht, apply_func_t apply_func)
{
	uint32_t idx;
	Bucket *p;
	int result;

	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);

	HASH_PROTECT_RECURSION(ht);
	for (idx = 0; idx < ht->nNumUsed; idx++) {
		p = ht->arData + idx;
		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
		result = apply_func(&p->val);

		if (result & ZEND_HASH_APPLY_REMOVE) {
			_zend_hash_del_el(ht, HT_IDX_TO_HASH(idx), p);
		}
		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
	}
	HASH_UNPROTECT_RECURSION(ht);
}
