SPL_METHOD(SplDoublyLinkedList, rewind)
{
	spl_dllist_object *intern = Z_SPLDLLIST_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_dllist_it_helper_rewind(&intern->traverse_pointer, &intern->traverse_position, intern->llist, intern->flags);
}
