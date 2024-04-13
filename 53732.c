ZEND_API int clean_non_persistent_class_full(zval *zv) /* {{{ */
{
	zend_class_entry *ce = Z_PTR_P(zv);
	return (ce->type == ZEND_INTERNAL_CLASS) ? ZEND_HASH_APPLY_KEEP : ZEND_HASH_APPLY_REMOVE;
}
/* }}} */
