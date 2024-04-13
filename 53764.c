static void zend_unclean_zval_ptr_dtor(zval *zv) /* {{{ */
{
	if (Z_TYPE_P(zv) == IS_INDIRECT) {
		zv = Z_INDIRECT_P(zv);
	}
	i_zval_ptr_dtor(zv ZEND_FILE_LINE_CC);
}
/* }}} */
