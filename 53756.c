ZEND_API zend_object *zend_get_this_object(zend_execute_data *ex) /* {{{ */
{
	while (ex) {
		if (Z_OBJ(ex->This)) {
			return Z_OBJ(ex->This);
		} else if (ex->func) {
			if (ex->func->type != ZEND_INTERNAL_FUNCTION || ex->func->common.scope) {
				return Z_OBJ(ex->This);
			}
		}
		ex = ex->prev_execute_data;
	}
	return NULL;
}
/* }}} */
