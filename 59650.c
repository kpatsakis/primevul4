ZEND_API void ZEND_FASTCALL zend_hash_iterator_del(uint32_t idx)
{
	HashTableIterator *iter = EG(ht_iterators) + idx;

	ZEND_ASSERT(idx != (uint32_t)-1);

	if (EXPECTED(iter->ht) && EXPECTED(iter->ht != HT_POISONED_PTR)
			&& EXPECTED(iter->ht->u.v.nIteratorsCount != 255)) {
		iter->ht->u.v.nIteratorsCount--;
	}
	iter->ht = NULL;

	if (idx == EG(ht_iterators_used) - 1) {
		while (idx > 0 && EG(ht_iterators)[idx - 1].ht == NULL) {
			idx--;
		}
		EG(ht_iterators_used) = idx;
	}
}
