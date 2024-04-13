static zend_always_inline uint32_t zend_array_dup_elements(HashTable *source, HashTable *target, int static_keys, int with_holes)
{
	uint32_t idx = 0;
	Bucket *p = source->arData;
	Bucket *q = target->arData;
	Bucket *end = p + source->nNumUsed;

	do {
		if (!zend_array_dup_element(source, target, idx, p, q, 0, static_keys, with_holes)) {
			uint32_t target_idx = idx;

			idx++; p++;
			while (p != end) {
				if (zend_array_dup_element(source, target, target_idx, p, q, 0, static_keys, with_holes)) {
					if (source->nInternalPointer == idx) {
						target->nInternalPointer = target_idx;
					}
					target_idx++; q++;
				}
				idx++; p++;
			}
			return target_idx;
		}
		idx++; p++; q++;
	} while (p != end);
	return idx;
}
