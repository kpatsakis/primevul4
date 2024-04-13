SPL_METHOD(SplDoublyLinkedList, pop)
{
	spl_dllist_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());
	spl_ptr_llist_pop(intern->llist, return_value);

	if (Z_ISUNDEF_P(return_value)) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't pop from an empty datastructure", 0);
		RETURN_NULL();
	}
}
