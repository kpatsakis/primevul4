ZEND_API int zval_update_constant_ex(zval *p, zend_bool inline_change, zend_class_entry *scope) /* {{{ */
{
	zval *const_value;
	char *colon;

	if (IS_CONSTANT_VISITED(p)) {
		zend_throw_error(NULL, "Cannot declare self-referencing constant '%s'", Z_STRVAL_P(p));
		return FAILURE;
	} else if (Z_TYPE_P(p) == IS_CONSTANT) {

		SEPARATE_ZVAL_NOREF(p);
		MARK_CONSTANT_VISITED(p);
		if (Z_CONST_FLAGS_P(p) & IS_CONSTANT_CLASS) {
			ZEND_ASSERT(EG(current_execute_data));
			if (inline_change) {
				zend_string_release(Z_STR_P(p));
			}
			if (EG(scope) && EG(scope)->name) {
				ZVAL_STR_COPY(p, EG(scope)->name);
			} else {
				ZVAL_EMPTY_STRING(p);
			}
		} else if (UNEXPECTED((const_value = zend_get_constant_ex(Z_STR_P(p), scope, Z_CONST_FLAGS_P(p))) == NULL)) {
			char *actual = Z_STRVAL_P(p);

			if (UNEXPECTED(EG(exception))) {
				RESET_CONSTANT_VISITED(p);
				return FAILURE;
			} else if ((colon = (char*)zend_memrchr(Z_STRVAL_P(p), ':', Z_STRLEN_P(p)))) {
				zend_throw_error(NULL, "Undefined class constant '%s'", Z_STRVAL_P(p));
				RESET_CONSTANT_VISITED(p);
				return FAILURE;
			} else {
				zend_string *save = Z_STR_P(p);
				char *slash;
				size_t actual_len = Z_STRLEN_P(p);
				if ((Z_CONST_FLAGS_P(p) & IS_CONSTANT_UNQUALIFIED) && (slash = (char *)zend_memrchr(actual, '\\', actual_len))) {
					actual = slash + 1;
					actual_len -= (actual - Z_STRVAL_P(p));
					if (inline_change) {
						zend_string *s = zend_string_init(actual, actual_len, 0);
						Z_STR_P(p) = s;
						Z_TYPE_FLAGS_P(p) = IS_TYPE_REFCOUNTED | IS_TYPE_COPYABLE;
					}
				}
				if (actual[0] == '\\') {
					if (inline_change) {
						memmove(Z_STRVAL_P(p), Z_STRVAL_P(p)+1, Z_STRLEN_P(p));
						--Z_STRLEN_P(p);
					} else {
						++actual;
					}
					--actual_len;
				}
				if ((Z_CONST_FLAGS_P(p) & IS_CONSTANT_UNQUALIFIED) == 0) {
					if (ZSTR_VAL(save)[0] == '\\') {
						zend_throw_error(NULL, "Undefined constant '%s'", ZSTR_VAL(save) + 1);
					} else {
						zend_throw_error(NULL, "Undefined constant '%s'", ZSTR_VAL(save));
					}
					if (inline_change) {
						zend_string_release(save);
					}
					RESET_CONSTANT_VISITED(p);
					return FAILURE;
				} else {
					zend_error(E_NOTICE, "Use of undefined constant %s - assumed '%s'",  actual,  actual);
					if (!inline_change) {
						ZVAL_STRINGL(p, actual, actual_len);
					} else {
						Z_TYPE_INFO_P(p) = Z_REFCOUNTED_P(p) ?
							IS_STRING_EX : IS_INTERNED_STRING_EX;
						if (save && ZSTR_VAL(save) != actual) {
							zend_string_release(save);
						}
					}
				}
			}
		} else {
			if (inline_change) {
				zend_string_release(Z_STR_P(p));
			}
			ZVAL_COPY_VALUE(p, const_value);
			if (Z_OPT_CONSTANT_P(p)) {
				if (UNEXPECTED(zval_update_constant_ex(p, 1, NULL) != SUCCESS)) {
					RESET_CONSTANT_VISITED(p);
					return FAILURE;
				}
			}
			zval_opt_copy_ctor(p);
		}
	} else if (Z_TYPE_P(p) == IS_CONSTANT_AST) {
		zval tmp;

		if (UNEXPECTED(zend_ast_evaluate(&tmp, Z_ASTVAL_P(p), scope) != SUCCESS)) {
			return FAILURE;
		}
		if (inline_change) {
			zval_ptr_dtor(p);
		}
		ZVAL_COPY_VALUE(p, &tmp);
	}
	return SUCCESS;
}
/* }}} */
