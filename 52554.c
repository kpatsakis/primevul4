zend_object_iterator *spl_dllist_get_iterator(zend_class_entry *ce, zval *object, int by_ref) /* {{{ */
{
	spl_dllist_it *iterator;
	spl_dllist_object *dllist_object = Z_SPLDLLIST_P(object);

	if (by_ref) {
		zend_throw_exception(spl_ce_RuntimeException, "An iterator cannot be used with foreach by reference", 0);
		return NULL;
	}

	iterator = emalloc(sizeof(spl_dllist_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_COPY(&iterator->intern.it.data, object);
	iterator->intern.it.funcs    = &spl_dllist_it_funcs;
	iterator->intern.ce          = ce;
	iterator->traverse_position  = dllist_object->traverse_position;
	iterator->traverse_pointer   = dllist_object->traverse_pointer;
	iterator->flags              = dllist_object->flags & SPL_DLLIST_IT_MASK;
	ZVAL_UNDEF(&iterator->intern.value);

	SPL_LLIST_CHECK_ADDREF(iterator->traverse_pointer);

	return &iterator->intern.it;
}
/* }}} */
