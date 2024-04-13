ZEND_API int zend_eval_stringl_ex(char *str, size_t str_len, zval *retval_ptr, char *string_name, int handle_exceptions) /* {{{ */
{
	int result;

	result = zend_eval_stringl(str, str_len, retval_ptr, string_name);
	if (handle_exceptions && EG(exception)) {
		zend_exception_error(EG(exception), E_ERROR);
		result = FAILURE;
	}
	return result;
}
/* }}} */
