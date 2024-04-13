ZEND_API int zend_set_local_var_str(const char *name, size_t len, zval *value, int force) /* {{{ */
{
	zend_execute_data *execute_data = EG(current_execute_data);

	while (execute_data && (!execute_data->func || !ZEND_USER_CODE(execute_data->func->common.type))) {
		execute_data = execute_data->prev_execute_data;
	}

	if (execute_data) {
		if (!execute_data->symbol_table) {
			zend_ulong h = zend_hash_func(name, len);
			zend_op_array *op_array = &execute_data->func->op_array;
			if (EXPECTED(op_array->last_var)) {
				zend_string **str = op_array->vars;
				zend_string **end = str + op_array->last_var;

				do {
					if (ZSTR_H(*str) == h &&
					    ZSTR_LEN(*str) == len &&
					    memcmp(ZSTR_VAL(*str), name, len) == 0) {
						zval *var = EX_VAR_NUM(str - op_array->vars);
						zval_ptr_dtor(var);
						ZVAL_COPY_VALUE(var, value);
						return SUCCESS;
					}
					str++;
				} while (str != end);
			}
			if (force) {
				zend_array *symbol_table = zend_rebuild_symbol_table();
				if (symbol_table) {
					return zend_hash_str_update(symbol_table, name, len, value) ? SUCCESS : FAILURE;;
				}
			}
		} else {
			return (zend_hash_str_update_ind(execute_data->symbol_table, name, len, value) != NULL) ? SUCCESS : FAILURE;
		}
	}
	return FAILURE;
}
/* }}} */
