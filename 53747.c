ZEND_API int zend_eval_stringl(char *str, size_t str_len, zval *retval_ptr, char *string_name) /* {{{ */
{
	zval pv;
	zend_op_array *new_op_array;
	uint32_t original_compiler_options;
	int retval;

	if (retval_ptr) {
		ZVAL_NEW_STR(&pv, zend_string_alloc(str_len + sizeof("return ;")-1, 1));
		memcpy(Z_STRVAL(pv), "return ", sizeof("return ") - 1);
		memcpy(Z_STRVAL(pv) + sizeof("return ") - 1, str, str_len);
		Z_STRVAL(pv)[Z_STRLEN(pv) - 1] = ';';
		Z_STRVAL(pv)[Z_STRLEN(pv)] = '\0';
	} else {
		ZVAL_STRINGL(&pv, str, str_len);
	}

	/*printf("Evaluating '%s'\n", pv.value.str.val);*/

	original_compiler_options = CG(compiler_options);
	CG(compiler_options) = ZEND_COMPILE_DEFAULT_FOR_EVAL;
	new_op_array = zend_compile_string(&pv, string_name);
	CG(compiler_options) = original_compiler_options;

	if (new_op_array) {
		zval local_retval;

		EG(no_extensions)=1;

		zend_try {
			ZVAL_UNDEF(&local_retval);
			zend_execute(new_op_array, &local_retval);
		} zend_catch {
			destroy_op_array(new_op_array);
			efree_size(new_op_array, sizeof(zend_op_array));
			zend_bailout();
		} zend_end_try();

		if (Z_TYPE(local_retval) != IS_UNDEF) {
			if (retval_ptr) {
				ZVAL_COPY_VALUE(retval_ptr, &local_retval);
			} else {
				zval_ptr_dtor(&local_retval);
			}
		} else {
			if (retval_ptr) {
				ZVAL_NULL(retval_ptr);
			}
		}

		EG(no_extensions)=0;
		destroy_op_array(new_op_array);
		efree_size(new_op_array, sizeof(zend_op_array));
		retval = SUCCESS;
	} else {
		retval = FAILURE;
	}
	zval_dtor(&pv);
	return retval;
}
/* }}} */
