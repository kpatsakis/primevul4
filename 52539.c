SPL_METHOD(SplDoublyLinkedList, bottom)
{
	zval *value;
	spl_dllist_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());
	value  = spl_ptr_llist_first(intern->llist);

	if (value == NULL || Z_ISUNDEF_P(value)) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't peek at an empty datastructure", 0);
		return;
	}

	ZVAL_DEREF(value);
	ZVAL_COPY(return_value, value);
}
