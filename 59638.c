ZEND_API int ZEND_FASTCALL zend_hash_get_current_key_type_ex(HashTable *ht, HashPosition *pos)
{
    uint32_t idx = *pos;
	Bucket *p;

	IS_CONSISTENT(ht);
	if (idx != HT_INVALID_IDX) {
		p = ht->arData + idx;
		if (p->key) {
			return HASH_KEY_IS_STRING;
		} else {
			return HASH_KEY_IS_LONG;
		}
	}
	return HASH_KEY_NON_EXISTENT;
}
