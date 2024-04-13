static void spl_ptr_llist_unshift(spl_ptr_llist *llist, zval *data) /* {{{ */
{
	spl_ptr_llist_element *elem = emalloc(sizeof(spl_ptr_llist_element));

	elem->rc   = 1;
	elem->prev = NULL;
	elem->next = llist->head;
	ZVAL_COPY_VALUE(&elem->data, data);

	if (llist->head) {
		llist->head->prev = elem;
	} else {
		llist->tail = elem;
	}

	llist->head = elem;
	llist->count++;

	if (llist->ctor) {
		llist->ctor(elem);
	}
}
/* }}} */
