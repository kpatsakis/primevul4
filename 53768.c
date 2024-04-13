ZEND_API int zval_update_constant(zval *pp, zend_bool inline_change) /* {{{ */
{
	return zval_update_constant_ex(pp, inline_change, NULL);
}
/* }}} */
