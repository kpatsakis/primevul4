void zend_verify_abstract_class(zend_class_entry *ce) /* {{{ */
{
	zend_function *func;
	zend_abstract_info ai;

	if ((ce->ce_flags & ZEND_ACC_IMPLICIT_ABSTRACT_CLASS) && !(ce->ce_flags & (ZEND_ACC_TRAIT | ZEND_ACC_EXPLICIT_ABSTRACT_CLASS))) {
		memset(&ai, 0, sizeof(ai));

		ZEND_HASH_FOREACH_PTR(&ce->function_table, func) {
			zend_verify_abstract_class_function(func, &ai);
		} ZEND_HASH_FOREACH_END();

		if (ai.cnt) {
			zend_error_noreturn(E_ERROR, "Class %s contains %d abstract method%s and must therefore be declared abstract or implement the remaining methods (" MAX_ABSTRACT_INFO_FMT MAX_ABSTRACT_INFO_FMT MAX_ABSTRACT_INFO_FMT ")",
				ZSTR_VAL(ce->name), ai.cnt,
				ai.cnt > 1 ? "s" : "",
				DISPLAY_ABSTRACT_FN(0),
				DISPLAY_ABSTRACT_FN(1),
				DISPLAY_ABSTRACT_FN(2)
				);
		}
	}
}
/* }}} */
