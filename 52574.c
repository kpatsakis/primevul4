static void spl_ptr_llist_pop(spl_ptr_llist *llist, zval *ret) /* {{{ */
{
	spl_ptr_llist_element    *tail = llist->tail;

	if (tail == NULL) {
		ZVAL_UNDEF(ret);
		return;
	}

	if (tail->prev) {
		tail->prev->next = NULL;
	} else {
		llist->head = NULL;
	}

	llist->tail = tail->prev;
	llist->count--;
	ZVAL_COPY(ret, &tail->data);

	if (llist->dtor) {
		llist->dtor(tail);
	}

	ZVAL_UNDEF(&tail->data);

	SPL_LLIST_DELREF(tail);
}
/* }}} */
