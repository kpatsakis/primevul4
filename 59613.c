ZEND_API uint32_t zend_array_count(HashTable *ht)
{
	uint32_t num;
	if (UNEXPECTED(ht->u.v.flags & HASH_FLAG_HAS_EMPTY_IND)) {
		num = zend_array_recalc_elements(ht);
		if (UNEXPECTED(ht->nNumOfElements == num)) {
			ht->u.v.flags &= ~HASH_FLAG_HAS_EMPTY_IND;
		}
	} else if (UNEXPECTED(ht == &EG(symbol_table))) {
		num = zend_array_recalc_elements(ht);
	} else {
		num = zend_hash_num_elements(ht);
	}
	return num;
}
