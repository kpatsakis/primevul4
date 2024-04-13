ZEND_API zval* ZEND_FASTCALL zend_hash_get_current_data_ex(HashTable *ht, HashPosition *pos)
{
	uint32_t idx = *pos;
	Bucket *p;

	IS_CONSISTENT(ht);
	if (idx != HT_INVALID_IDX) {
		p = ht->arData + idx;
		return &p->val;
	} else {
		return NULL;
	}
}
