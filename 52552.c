SPL_METHOD(SplDoublyLinkedList, add)
{
	zval                  *zindex, *value;
	spl_dllist_object     *intern;
	spl_ptr_llist_element *element;
	zend_long                  index;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &zindex, &value) == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());
	index  = spl_offset_convert_to_long(zindex);

	if (index < 0 || index > intern->llist->count) {
		zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid or out of range", 0);
		return;
	}

	if (Z_REFCOUNTED_P(value)) {
		Z_ADDREF_P(value);
	}
	if (index == intern->llist->count) {
		/* If index is the last entry+1 then we do a push because we're not inserting before any entry */
		spl_ptr_llist_push(intern->llist, value);
	} else {
		/* Create the new element we want to insert */
		spl_ptr_llist_element *elem = emalloc(sizeof(spl_ptr_llist_element));

		/* Get the element we want to insert before */
		element = spl_ptr_llist_offset(intern->llist, index, intern->flags & SPL_DLLIST_IT_LIFO);

		ZVAL_COPY_VALUE(&elem->data, value);
		elem->rc   = 1;
		/* connect to the neighbours */
		elem->next = element;
		elem->prev = element->prev;

		/* connect the neighbours to this new element */
		if (elem->prev == NULL) {
			intern->llist->head = elem;
		} else {
			element->prev->next = elem;
		}
		element->prev = elem;

		intern->llist->count++;

		if (intern->llist->ctor) {
			intern->llist->ctor(elem);
		}
	}
} /* }}} */

/* {{{ iterator handler table */
