static void spl_dllist_it_get_current_key(zend_object_iterator *iter, zval *key) /* {{{ */
{
	spl_dllist_it *iterator = (spl_dllist_it *)iter;

	ZVAL_LONG(key, iterator->traverse_position);
}
/* }}} */
