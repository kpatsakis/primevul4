static void spl_dllist_it_dtor(zend_object_iterator *iter) /* {{{ */
{
	spl_dllist_it *iterator = (spl_dllist_it *)iter;

	SPL_LLIST_CHECK_DELREF(iterator->traverse_pointer);

	zend_user_it_invalidate_current(iter);
	zval_ptr_dtor(&iterator->intern.it.data);
}
/* }}} */
