static void spl_dllist_it_helper_rewind(spl_ptr_llist_element **traverse_pointer_ptr, int *traverse_position_ptr, spl_ptr_llist *llist, int flags) /* {{{ */
{
	SPL_LLIST_CHECK_DELREF(*traverse_pointer_ptr);

	if (flags & SPL_DLLIST_IT_LIFO) {
		*traverse_position_ptr = llist->count-1;
		*traverse_pointer_ptr  = llist->tail;
	} else {
		*traverse_position_ptr = 0;
		*traverse_pointer_ptr  = llist->head;
	}

	SPL_LLIST_CHECK_ADDREF(*traverse_pointer_ptr);
}
/* }}} */
