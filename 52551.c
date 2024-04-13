SPL_METHOD(SplDoublyLinkedList, unserialize)
{
	spl_dllist_object *intern = Z_SPLDLLIST_P(getThis());
	zval *flags, *elem;
	char *buf;
	size_t buf_len;
	const unsigned char *p, *s;
	php_unserialize_data_t var_hash;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &buf, &buf_len) == FAILURE) {
		return;
	}

	if (buf_len == 0) {
		return;
	}

	s = p = (const unsigned char*)buf;
	PHP_VAR_UNSERIALIZE_INIT(var_hash);

	/* flags */
	flags = var_tmp_var(&var_hash);
	if (!php_var_unserialize(flags, &p, s + buf_len, &var_hash) || Z_TYPE_P(flags) != IS_LONG) {
		goto error;
	}

	intern->flags = (int)Z_LVAL_P(flags);

	/* elements */
	while(*p == ':') {
		++p;
		elem = var_tmp_var(&var_hash);
		if (!php_var_unserialize(elem, &p, s + buf_len, &var_hash)) {
			goto error;
		}
		var_push_dtor(&var_hash, elem);

		spl_ptr_llist_push(intern->llist, elem);
	}

	if (*p != '\0') {
		goto error;
	}

	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	return;

error:
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0, "Error at offset %pd of %d bytes", (zend_long)((char*)p - buf), buf_len);
	return;

} /* }}} */

/* {{{ proto void SplDoublyLinkedList::add(mixed index, mixed newval)
