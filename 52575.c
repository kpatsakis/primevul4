static void spl_ptr_llist_push(spl_ptr_llist *llist, zval *data) /* {{{ */
{
	spl_ptr_llist_element *elem = emalloc(sizeof(spl_ptr_llist_element));

	elem->rc   = 1;
	elem->prev = llist->tail;
	elem->next = NULL;
	ZVAL_COPY_VALUE(&elem->data, data);

	if (llist->tail) {
		llist->tail->next = elem;
	} else {
		llist->head = elem;
	}

	llist->tail = elem;
	llist->count++;

	if (llist->ctor) {
		llist->ctor(elem);
	}
}
/* }}} */
