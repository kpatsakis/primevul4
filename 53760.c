ZEND_API zend_array *zend_rebuild_symbol_table(void) /* {{{ */
{
	zend_execute_data *ex;
	zend_array *symbol_table;

	/* Search for last called user function */
	ex = EG(current_execute_data);
	while (ex && (!ex->func || !ZEND_USER_CODE(ex->func->common.type))) {
		ex = ex->prev_execute_data;
	}
	if (!ex) {
		return NULL;
	}
	if (ex->symbol_table) {
		return ex->symbol_table;
	}

	if (EG(symtable_cache_ptr) >= EG(symtable_cache)) {
		/*printf("Cache hit!  Reusing %x\n", symtable_cache[symtable_cache_ptr]);*/
		symbol_table = ex->symbol_table = *(EG(symtable_cache_ptr)--);
		if (!ex->func->op_array.last_var) {
			return symbol_table;
		}
		zend_hash_extend(symbol_table, ex->func->op_array.last_var, 0);
	} else {
		symbol_table = ex->symbol_table = emalloc(sizeof(zend_array));
		zend_hash_init(symbol_table, ex->func->op_array.last_var, NULL, ZVAL_PTR_DTOR, 0);
		if (!ex->func->op_array.last_var) {
			return symbol_table;
		}
		zend_hash_real_init(symbol_table, 0);
		/*printf("Cache miss!  Initialized %x\n", EG(active_symbol_table));*/
	}
	if (EXPECTED(ex->func->op_array.last_var)) {
		zend_string **str = ex->func->op_array.vars;
		zend_string **end = str + ex->func->op_array.last_var;
		zval *var = ZEND_CALL_VAR_NUM(ex, 0);

		do {
			_zend_hash_append_ind(symbol_table, *str, var);
			str++;
			var++;
		} while (str != end);
	}
	return symbol_table;
}
/* }}} */
