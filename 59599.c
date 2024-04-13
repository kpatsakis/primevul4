ZEND_API void ZEND_FASTCALL _zend_hash_init_ex(HashTable *ht, uint32_t nSize, dtor_func_t pDestructor, zend_bool persistent, zend_bool bApplyProtection ZEND_FILE_LINE_DC)
{
	_zend_hash_init(ht, nSize, pDestructor, persistent ZEND_FILE_LINE_RELAY_CC);
	if (!bApplyProtection) {
		ht->u.flags &= ~HASH_FLAG_APPLY_PROTECTION;
	}
}
