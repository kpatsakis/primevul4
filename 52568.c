static void spl_ptr_llist_copy(spl_ptr_llist *from, spl_ptr_llist *to) /* {{{ */
{
	spl_ptr_llist_element *current = from->head, *next;

	while (current) {
		next = current->next;

		/*??? FIXME
		if (ctor) {
			ctor(current);
		}
		*/

		spl_ptr_llist_push(to, &current->data);
		current = next;
	}

}
/* }}} */
