static zend_always_inline void zend_array_dup_packed_elements(HashTable *source, HashTable *target, int with_holes)
{
	Bucket *p = source->arData;
	Bucket *q = target->arData;
	Bucket *end = p + source->nNumUsed;

	do {
		if (!zend_array_dup_element(source, target, 0, p, q, 1, 1, with_holes)) {
			if (with_holes) {
				ZVAL_UNDEF(&q->val);
			}
		}
		p++; q++;
	} while (p != end);
}
