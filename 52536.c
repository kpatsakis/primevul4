SPL_METHOD(SplDoublyLinkedList, unshift)
{
	zval *value;
	spl_dllist_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &value) == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());
	spl_ptr_llist_unshift(intern->llist, value);

	RETURN_TRUE;
}
