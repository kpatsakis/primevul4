SPL_METHOD(SplDoublyLinkedList, shift)
{
	spl_dllist_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());
	spl_ptr_llist_shift(intern->llist, return_value);

	if (Z_ISUNDEF_P(return_value)) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't shift from an empty datastructure", 0);
		RETURN_NULL();
	}
}
