static HashTable *spl_dllist_object_get_gc(zval *obj, zval **gc_data, int *gc_data_count) /* {{{ */
{
	spl_dllist_object *intern  = Z_SPLDLLIST_P(obj);
	spl_ptr_llist_element *current = intern->llist->head;
	int i = 0;

	if (intern->gc_data_count < intern->llist->count) {
		intern->gc_data_count = intern->llist->count;
		intern->gc_data = safe_erealloc(intern->gc_data, intern->gc_data_count, sizeof(zval), 0);
	}

	while (current) {
		ZVAL_COPY_VALUE(&intern->gc_data[i++], &current->data);
		current = current->next;
	}

	*gc_data = intern->gc_data;
	*gc_data_count = i;
	return zend_std_get_properties(obj);
}
/* }}} */
