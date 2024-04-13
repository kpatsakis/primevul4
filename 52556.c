static zval *spl_dllist_it_get_current_data(zend_object_iterator *iter) /* {{{ */
{
	spl_dllist_it         *iterator = (spl_dllist_it *)iter;
	spl_ptr_llist_element *element  = iterator->traverse_pointer;

	if (element == NULL || Z_ISUNDEF(element->data)) {
		return NULL;
	}

	return &element->data;
}
/* }}} */
