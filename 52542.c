SPL_METHOD(SplDoublyLinkedList, getIteratorMode)
{
	spl_dllist_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());

	RETURN_LONG(intern->flags);
}
