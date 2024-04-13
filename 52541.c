SPL_METHOD(SplDoublyLinkedList, isEmpty)
{
	zend_long count;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_dllist_object_count_elements(getThis(), &count);
	RETURN_BOOL(count == 0);
}
