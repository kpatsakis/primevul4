SPL_METHOD(SplDoublyLinkedList, count)
{
	zend_long count;
	spl_dllist_object *intern = Z_SPLDLLIST_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	count = spl_ptr_llist_count(intern->llist);
	RETURN_LONG(count);
}
