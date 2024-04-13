SPL_METHOD(SplDoublyLinkedList, current)
{
	spl_dllist_object     *intern  = Z_SPLDLLIST_P(getThis());
	spl_ptr_llist_element *element = intern->traverse_pointer;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (element == NULL || Z_ISUNDEF(element->data)) {
		RETURN_NULL();
	} else {
		zval *value = &element->data;

		ZVAL_DEREF(value);
		ZVAL_COPY(return_value, value);
	}
}
