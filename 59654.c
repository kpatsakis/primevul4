ZEND_API void ZEND_FASTCALL zend_hash_merge_ex(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, merge_checker_func_t pMergeSource, void *pParam)
{
	uint32_t idx;
	Bucket *p;
	zval *t;

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);
	HT_ASSERT(GC_REFCOUNT(target) == 1);

	for (idx = 0; idx < source->nNumUsed; idx++) {
		p = source->arData + idx;
		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
		if (zend_hash_replace_checker_wrapper(target, &p->val, p, pParam, pMergeSource)) {
			t = zend_hash_update(target, p->key, &p->val);
			if (t && pCopyConstructor) {
				pCopyConstructor(t);
			}
		}
	}
	if (target->nNumOfElements > 0) {
		idx = 0;
		while (Z_TYPE(target->arData[idx].val) == IS_UNDEF) {
			idx++;
		}
		target->nInternalPointer = idx;
	}
}
