static zend_never_inline void ZEND_FASTCALL _zend_hash_iterators_remove(HashTable *ht)
{
	HashTableIterator *iter = EG(ht_iterators);
	HashTableIterator *end  = iter + EG(ht_iterators_used);

	while (iter != end) {
		if (iter->ht == ht) {
			iter->ht = HT_POISONED_PTR;
		}
		iter++;
	}
}
