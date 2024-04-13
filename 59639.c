ZEND_API void ZEND_FASTCALL zend_hash_get_current_key_zval_ex(const HashTable *ht, zval *key, HashPosition *pos)
{
	uint32_t idx = *pos;
	Bucket *p;

	IS_CONSISTENT(ht);
	if (idx == HT_INVALID_IDX) {
		ZVAL_NULL(key);
	} else {
		p = ht->arData + idx;
		if (p->key) {
			ZVAL_STR_COPY(key, p->key);
		} else {
			ZVAL_LONG(key, p->h);
		}
	}
}
