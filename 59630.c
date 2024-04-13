ZEND_API void ZEND_FASTCALL zend_hash_del_bucket(HashTable *ht, Bucket *p)
{
	IS_CONSISTENT(ht);
	HT_ASSERT(GC_REFCOUNT(ht) == 1);
	_zend_hash_del_el(ht, HT_IDX_TO_HASH(p - ht->arData), p);
}
