static void spl_ptr_llist_zval_dtor(spl_ptr_llist_element *elem) { /* {{{ */
	if (!Z_ISUNDEF(elem->data)) {
		zval_ptr_dtor(&elem->data);
		ZVAL_UNDEF(&elem->data);
	}
}
/* }}} */
