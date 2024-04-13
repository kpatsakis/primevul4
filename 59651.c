ZEND_API HashPosition ZEND_FASTCALL zend_hash_iterator_pos(uint32_t idx, HashTable *ht)
{
	HashTableIterator *iter = EG(ht_iterators) + idx;

	ZEND_ASSERT(idx != (uint32_t)-1);
	if (iter->pos == HT_INVALID_IDX) {
		return HT_INVALID_IDX;
	} else if (UNEXPECTED(iter->ht != ht)) {
		if (EXPECTED(iter->ht) && EXPECTED(iter->ht != HT_POISONED_PTR)
				&& EXPECTED(iter->ht->u.v.nIteratorsCount != 255)) {
			iter->ht->u.v.nIteratorsCount--;
		}
		if (EXPECTED(ht->u.v.nIteratorsCount != 255)) {
			ht->u.v.nIteratorsCount++;
		}
		iter->ht = ht;
		iter->pos = ht->nInternalPointer;
	}
	return iter->pos;
}
