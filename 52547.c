SPL_METHOD(SplDoublyLinkedList, valid)
{
	spl_dllist_object *intern = Z_SPLDLLIST_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(intern->traverse_pointer != NULL);
}
