SPL_METHOD(SplDoublyLinkedList, key)
{
	spl_dllist_object *intern = Z_SPLDLLIST_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(intern->traverse_position);
}
