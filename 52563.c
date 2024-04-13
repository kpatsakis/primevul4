static void spl_dllist_object_free_storage(zend_object *object) /* {{{ */
{
	spl_dllist_object *intern = spl_dllist_from_obj(object);
	zval tmp;

	zend_object_std_dtor(&intern->std);

	while (intern->llist->count > 0) {
		spl_ptr_llist_pop(intern->llist, &tmp);
		zval_ptr_dtor(&tmp);
	}

	if (intern->gc_data != NULL) {
		efree(intern->gc_data);
	};

	spl_ptr_llist_destroy(intern->llist);
	SPL_LLIST_CHECK_DELREF(intern->traverse_pointer);
}
/* }}} */
