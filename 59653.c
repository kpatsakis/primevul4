static zend_always_inline void zend_hash_iterators_remove(HashTable *ht)
{
	if (UNEXPECTED(ht->u.v.nIteratorsCount)) {
		_zend_hash_iterators_remove(ht);
	}
}
