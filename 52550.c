SPL_METHOD(SplDoublyLinkedList, serialize)
{
	spl_dllist_object     *intern   = Z_SPLDLLIST_P(getThis());
	smart_str              buf      = {0};
	spl_ptr_llist_element *current  = intern->llist->head, *next;
	zval                   flags;
	php_serialize_data_t   var_hash;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	PHP_VAR_SERIALIZE_INIT(var_hash);

	/* flags */
	ZVAL_LONG(&flags, intern->flags);
	php_var_serialize(&buf, &flags, &var_hash);
	zval_ptr_dtor(&flags);

	/* elements */
	while (current) {
		smart_str_appendc(&buf, ':');
		next = current->next;

		php_var_serialize(&buf, &current->data, &var_hash);

		current = next;
	}

	smart_str_0(&buf);

	/* done */
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	if (buf.s) {
		RETURN_NEW_STR(buf.s);
	} else {
		RETURN_NULL();
	}

} /* }}} */

/* {{{ proto void SplDoublyLinkedList::unserialize(string serialized)
