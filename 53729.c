ZEND_API void _zval_internal_ptr_dtor(zval *zval_ptr ZEND_FILE_LINE_DC) /* {{{ */
{
	if (Z_REFCOUNTED_P(zval_ptr)) {
		Z_DELREF_P(zval_ptr);
		if (Z_REFCOUNT_P(zval_ptr) == 0) {
			_zval_internal_dtor_for_ptr(zval_ptr ZEND_FILE_LINE_CC);
		}
	}
}
/* }}} */
