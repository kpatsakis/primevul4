SPL_METHOD(SplDoublyLinkedList, offsetUnset)
{
	zval                  *zindex;
	zend_long             index;
	spl_dllist_object     *intern;
	spl_ptr_llist_element *element;
	spl_ptr_llist         *llist;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zindex) == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());
	index  = spl_offset_convert_to_long(zindex);
	llist  = intern->llist;

	if (index < 0 || index >= intern->llist->count) {
		zend_throw_exception(spl_ce_OutOfRangeException, "Offset out of range", 0);
		return;
	}

	element = spl_ptr_llist_offset(intern->llist, index, intern->flags & SPL_DLLIST_IT_LIFO);

	if (element != NULL) {
		/* connect the neightbors */
		if (element->prev) {
			element->prev->next = element->next;
		}

		if (element->next) {
			element->next->prev = element->prev;
		}

		/* take care of head/tail */
		if (element == llist->head) {
			llist->head = element->next;
		}

		if (element == llist->tail) {
			llist->tail = element->prev;
		}

		/* finally, delete the element */
		llist->count--;

		if(llist->dtor) {
			llist->dtor(element);
		}

		if (intern->traverse_pointer == element) {
			SPL_LLIST_DELREF(element);
			intern->traverse_pointer = NULL;
		}
		zval_ptr_dtor(&element->data);
		ZVAL_UNDEF(&element->data);

		SPL_LLIST_DELREF(element);
	} else {
		zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid", 0);
		return;
	}
} /* }}} */

static void spl_dllist_it_dtor(zend_object_iterator *iter) /* {{{ */
