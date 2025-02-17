static uint32_t zend_array_recalc_elements(HashTable *ht)
{
       zval *val;
       uint32_t num = ht->nNumOfElements;

	   ZEND_HASH_FOREACH_VAL(ht, val) {
		   if (Z_TYPE_P(val) == IS_UNDEF) continue;
		   if (Z_TYPE_P(val) == IS_INDIRECT) {
			   if (UNEXPECTED(Z_TYPE_P(Z_INDIRECT_P(val)) == IS_UNDEF)) {
				   num--;
			   }
		   }
       } ZEND_HASH_FOREACH_END();
       return num;
}
