ZEND_API const char *get_active_class_name(const char **space) /* {{{ */
{
	zend_function *func;

	if (!zend_is_executing()) {
		if (space) {
			*space = "";
		}
		return "";
	}

	func = EG(current_execute_data)->func;
	switch (func->type) {
		case ZEND_USER_FUNCTION:
		case ZEND_INTERNAL_FUNCTION:
		{
			zend_class_entry *ce = func->common.scope;

			if (space) {
				*space = ce ? "::" : "";
			}
			return ce ? ZSTR_VAL(ce->name) : "";
		}
		default:
			if (space) {
				*space = "";
			}
			return "";
	}
}
/* }}} */
