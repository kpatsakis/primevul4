ZEND_API HashPosition ZEND_FASTCALL zend_hash_iterators_lower_pos(HashTable *ht, HashPosition start)
{
	HashTableIterator *iter = EG(ht_iterators);
	HashTableIterator *end  = iter + EG(ht_iterators_used);
	HashPosition res = HT_INVALID_IDX;

	while (iter != end) {
		if (iter->ht == ht) {
			if (iter->pos >= start && iter->pos < res) {
				res = iter->pos;
			}
		}
		iter++;
	}
	return res;
}
