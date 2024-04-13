static zval *spl_ptr_llist_first(spl_ptr_llist *llist) /* {{{ */
{
	spl_ptr_llist_element *head = llist->head;

	if (head == NULL) {
		return NULL;
	} else {
		return &head->data;
	}
}
/* }}} */
