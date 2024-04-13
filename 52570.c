static void spl_ptr_llist_destroy(spl_ptr_llist *llist) /* {{{ */
{
	spl_ptr_llist_element   *current = llist->head, *next;
	spl_ptr_llist_dtor_func  dtor    = llist->dtor;

	while (current) {
		next = current->next;
		if (dtor) {
			dtor(current);
		}
		SPL_LLIST_DELREF(current);
		current = next;
	}

	efree(llist);
}
/* }}} */
