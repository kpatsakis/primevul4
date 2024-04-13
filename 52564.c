static HashTable* spl_dllist_object_get_debug_info(zval *obj, int *is_temp) /* {{{{ */
{
	spl_dllist_object     *intern  = Z_SPLDLLIST_P(obj);
	spl_ptr_llist_element *current = intern->llist->head, *next;
	zval tmp, dllist_array;
	zend_string *pnstr;
	int  i = 0;
	HashTable *debug_info;
	*is_temp = 1;

	if (!intern->std.properties) {
		rebuild_object_properties(&intern->std);
	}

	ALLOC_HASHTABLE(debug_info);
	zend_hash_init(debug_info, 1, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(debug_info, intern->std.properties, (copy_ctor_func_t) zval_add_ref);

	pnstr = spl_gen_private_prop_name(spl_ce_SplDoublyLinkedList, "flags", sizeof("flags")-1);
	ZVAL_LONG(&tmp, intern->flags);
	zend_hash_add(debug_info, pnstr, &tmp);
	zend_string_release(pnstr);

	array_init(&dllist_array);

	while (current) {
		next = current->next;

		add_index_zval(&dllist_array, i, &current->data);
		if (Z_REFCOUNTED(current->data)) {
			Z_ADDREF(current->data);
		}
		i++;

		current = next;
	}

	pnstr = spl_gen_private_prop_name(spl_ce_SplDoublyLinkedList, "dllist", sizeof("dllist")-1);
	zend_hash_add(debug_info, pnstr, &dllist_array);
	zend_string_release(pnstr);

	return debug_info;
}
/* }}}} */
