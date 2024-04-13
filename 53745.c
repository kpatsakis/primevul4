ZEND_API int zend_eval_string(char *str, zval *retval_ptr, char *string_name) /* {{{ */
{
	return zend_eval_stringl(str, strlen(str), retval_ptr, string_name);
}
/* }}} */
