static void spl_ptr_llist_shift(spl_ptr_llist *llist, zval *ret) /* {{{ */
{
	spl_ptr_llist_element   *head = llist->head;

	if (head == NULL) {
		ZVAL_UNDEF(ret);
		return;
	}

	if (head->next) {
		head->next->prev = NULL;
	} else {
		llist->tail = NULL;
	}

	llist->head = head->next;
	llist->count--;
	ZVAL_COPY(ret, &head->data);

	if (llist->dtor) {
		llist->dtor(head);
	}
	ZVAL_UNDEF(&head->data);

	SPL_LLIST_DELREF(head);
}
/* }}} */
