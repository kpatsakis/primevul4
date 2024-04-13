static void spl_ptr_llist_zval_ctor(spl_ptr_llist_element *elem) { /* {{{ */
	if (Z_REFCOUNTED(elem->data)) {
		Z_ADDREF(elem->data);
	}
}
/* }}} */
