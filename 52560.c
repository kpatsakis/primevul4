static void spl_dllist_it_rewind(zend_object_iterator *iter) /* {{{ */
{
	spl_dllist_it *iterator = (spl_dllist_it *)iter;
	spl_dllist_object *object = Z_SPLDLLIST_P(&iter->data);
	spl_ptr_llist *llist = object->llist;

	spl_dllist_it_helper_rewind(&iterator->traverse_pointer, &iterator->traverse_position, llist, object->flags);
}
/* }}} */
