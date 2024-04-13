static int spl_dllist_it_valid(zend_object_iterator *iter) /* {{{ */
{
	spl_dllist_it         *iterator = (spl_dllist_it *)iter;
	spl_ptr_llist_element *element  = iterator->traverse_pointer;

	return (element != NULL ? SUCCESS : FAILURE);
}
/* }}} */
