static spl_ptr_llist_element *spl_ptr_llist_offset(spl_ptr_llist *llist, zend_long offset, int backward) /* {{{ */
{

	spl_ptr_llist_element *current;
	int pos = 0;

	if (backward) {
		current = llist->tail;
	} else {
		current = llist->head;
	}

	while (current && pos < offset) {
		pos++;
		if (backward) {
			current = current->prev;
		} else {
			current = current->next;
		}
	}

	return current;
}
/* }}} */
