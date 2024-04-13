ZEND_API void zend_attach_symbol_table(zend_execute_data *execute_data) /* {{{ */
{
	zend_op_array *op_array = &execute_data->func->op_array;
	HashTable *ht = execute_data->symbol_table;

	/* copy real values from symbol table into CV slots and create
	   INDIRECT references to CV in symbol table  */
	if (EXPECTED(op_array->last_var)) {
		zend_string **str = op_array->vars;
		zend_string **end = str + op_array->last_var;
		zval *var = EX_VAR_NUM(0);

		do {
			zval *zv = zend_hash_find(ht, *str);

			if (zv) {
				if (Z_TYPE_P(zv) == IS_INDIRECT) {
					zval *val = Z_INDIRECT_P(zv);

					ZVAL_COPY_VALUE(var, val);
				} else {
					ZVAL_COPY_VALUE(var, zv);
				}
			} else {
				ZVAL_UNDEF(var);
				zv = zend_hash_add_new(ht, *str, var);
			}
			ZVAL_INDIRECT(zv, var);
			str++;
			var++;
		} while (str != end);
	}
}
/* }}} */
