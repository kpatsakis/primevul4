ZEND_API const char *get_active_function_name(void) /* {{{ */
{
	zend_function *func;

	if (!zend_is_executing()) {
		return NULL;
	}
	func = EG(current_execute_data)->func;
	switch (func->type) {
		case ZEND_USER_FUNCTION: {
				zend_string *function_name = func->common.function_name;

				if (function_name) {
					return ZSTR_VAL(function_name);
				} else {
					return "main";
				}
			}
			break;
		case ZEND_INTERNAL_FUNCTION:
			return ZSTR_VAL(func->common.function_name);
			break;
		default:
			return NULL;
	}
}
/* }}} */
