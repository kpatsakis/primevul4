ZEND_API void ZEND_FASTCALL zend_hash_set_apply_protection(HashTable *ht, zend_bool bApplyProtection)
{
	if (bApplyProtection) {
		ht->u.flags |= HASH_FLAG_APPLY_PROTECTION;
	} else {
		ht->u.flags &= ~HASH_FLAG_APPLY_PROTECTION;
	}
}
