SPL_METHOD(SplDoublyLinkedList, prev)
{
	spl_dllist_object *intern = Z_SPLDLLIST_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_dllist_it_helper_move_forward(&intern->traverse_pointer, &intern->traverse_position, intern->llist, intern->flags ^ SPL_DLLIST_IT_LIFO);
}
