void shutdown_destructors(void) /* {{{ */
{
	if (CG(unclean_shutdown)) {
		EG(symbol_table).pDestructor = zend_unclean_zval_ptr_dtor;
	}
	zend_try {
		uint32_t symbols;
		do {
			symbols = zend_hash_num_elements(&EG(symbol_table));
			zend_hash_reverse_apply(&EG(symbol_table), (apply_func_t) zval_call_destructor);
		} while (symbols != zend_hash_num_elements(&EG(symbol_table)));
		zend_objects_store_call_destructors(&EG(objects_store));
	} zend_catch {
		/* if we couldn't destruct cleanly, mark all objects as destructed anyway */
		zend_objects_store_mark_destructed(&EG(objects_store));
	} zend_end_try();
}
/* }}} */
