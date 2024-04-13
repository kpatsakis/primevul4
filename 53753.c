ZEND_API zend_class_entry *zend_get_called_scope(zend_execute_data *ex) /* {{{ */
{
	while (ex) {
		if (ex->called_scope) {
			return ex->called_scope;
		} else if (ex->func) {
			if (ex->func->type != ZEND_INTERNAL_FUNCTION || ex->func->common.scope) {
				return ex->called_scope;
			}
		}
		ex = ex->prev_execute_data;
	}
	return NULL;
}
/* }}} */
