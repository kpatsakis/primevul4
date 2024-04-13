ZEND_API zend_bool zend_is_executing(void) /* {{{ */
{
	return EG(current_execute_data) != 0;
}
/* }}} */
