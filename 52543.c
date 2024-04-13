SPL_METHOD(SplDoublyLinkedList, offsetGet)
{
	zval                  *zindex;
	zend_long                   index;
	spl_dllist_object     *intern;
	spl_ptr_llist_element *element;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zindex) == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());
	index  = spl_offset_convert_to_long(zindex);

	if (index < 0 || index >= intern->llist->count) {
		zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid or out of range", 0);
		return;
	}

	element = spl_ptr_llist_offset(intern->llist, index, intern->flags & SPL_DLLIST_IT_LIFO);

	if (element != NULL) {
		zval *value = &element->data;

		ZVAL_DEREF(value);
		ZVAL_COPY(return_value, value);
	} else {
		zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid", 0);
	}
} /* }}} */

/* {{{ proto void SplDoublyLinkedList::offsetSet(mixed index, mixed newval)
