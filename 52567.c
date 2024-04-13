static zend_object *spl_dllist_object_new_ex(zend_class_entry *class_type, zval *orig, int clone_orig) /* {{{ */
{
	spl_dllist_object *intern;
	zend_class_entry  *parent = class_type;
	int                inherited = 0;

	intern = ecalloc(1, sizeof(spl_dllist_object) + zend_object_properties_size(parent));

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	intern->flags = 0;
	intern->traverse_position = 0;

	if (orig) {
		spl_dllist_object *other = Z_SPLDLLIST_P(orig);
		intern->ce_get_iterator = other->ce_get_iterator;

		if (clone_orig) {
			intern->llist = (spl_ptr_llist *)spl_ptr_llist_init(other->llist->ctor, other->llist->dtor);
			spl_ptr_llist_copy(other->llist, intern->llist);
			intern->traverse_pointer  = intern->llist->head;
			SPL_LLIST_CHECK_ADDREF(intern->traverse_pointer);
		} else {
			intern->llist = other->llist;
			intern->traverse_pointer  = intern->llist->head;
			SPL_LLIST_CHECK_ADDREF(intern->traverse_pointer);
		}

		intern->flags = other->flags;
	} else {
		intern->llist = (spl_ptr_llist *)spl_ptr_llist_init(spl_ptr_llist_zval_ctor, spl_ptr_llist_zval_dtor);
		intern->traverse_pointer  = intern->llist->head;
		SPL_LLIST_CHECK_ADDREF(intern->traverse_pointer);
	}

	while (parent) {
		if (parent == spl_ce_SplStack) {
			intern->flags |= (SPL_DLLIST_IT_FIX | SPL_DLLIST_IT_LIFO);
			intern->std.handlers = &spl_handler_SplDoublyLinkedList;
		} else if (parent == spl_ce_SplQueue) {
			intern->flags |= SPL_DLLIST_IT_FIX;
			intern->std.handlers = &spl_handler_SplDoublyLinkedList;
		}

		if (parent == spl_ce_SplDoublyLinkedList) {
			intern->std.handlers = &spl_handler_SplDoublyLinkedList;
			break;
		}

		parent = parent->parent;
		inherited = 1;
	}

	if (!parent) { /* this must never happen */
		php_error_docref(NULL, E_COMPILE_ERROR, "Internal compiler error, Class is not child of SplDoublyLinkedList");
	}
	if (inherited) {
		intern->fptr_offset_get = zend_hash_str_find_ptr(&class_type->function_table, "offsetget", sizeof("offsetget") - 1);
		if (intern->fptr_offset_get->common.scope == parent) {
			intern->fptr_offset_get = NULL;
		}
		intern->fptr_offset_set = zend_hash_str_find_ptr(&class_type->function_table, "offsetset", sizeof("offsetset") - 1);
		if (intern->fptr_offset_set->common.scope == parent) {
			intern->fptr_offset_set = NULL;
		}
		intern->fptr_offset_has = zend_hash_str_find_ptr(&class_type->function_table, "offsetexists", sizeof("offsetexists") - 1);
		if (intern->fptr_offset_has->common.scope == parent) {
			intern->fptr_offset_has = NULL;
		}
		intern->fptr_offset_del = zend_hash_str_find_ptr(&class_type->function_table, "offsetunset", sizeof("offsetunset") - 1);
		if (intern->fptr_offset_del->common.scope == parent) {
			intern->fptr_offset_del = NULL;
		}
		intern->fptr_count = zend_hash_str_find_ptr(&class_type->function_table, "count", sizeof("count") - 1);
		if (intern->fptr_count->common.scope == parent) {
			intern->fptr_count = NULL;
		}
	}

	return &intern->std;
}
/* }}} */
