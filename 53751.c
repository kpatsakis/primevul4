zend_class_entry *zend_fetch_class(zend_string *class_name, int fetch_type) /* {{{ */
{
	zend_class_entry *ce;
	int fetch_sub_type = fetch_type & ZEND_FETCH_CLASS_MASK;

check_fetch_type:
	switch (fetch_sub_type) {
		case ZEND_FETCH_CLASS_SELF:
			if (UNEXPECTED(!EG(scope))) {
				zend_throw_or_error(fetch_type, NULL, "Cannot access self:: when no class scope is active");
			}
			return EG(scope);
		case ZEND_FETCH_CLASS_PARENT:
			if (UNEXPECTED(!EG(scope))) {
				zend_throw_or_error(fetch_type, NULL, "Cannot access parent:: when no class scope is active");
				return NULL;
			}
			if (UNEXPECTED(!EG(scope)->parent)) {
				zend_throw_or_error(fetch_type, NULL, "Cannot access parent:: when current class scope has no parent");
			}
			return EG(scope)->parent;
		case ZEND_FETCH_CLASS_STATIC:
			ce = zend_get_called_scope(EG(current_execute_data));
			if (UNEXPECTED(!ce)) {
				zend_throw_or_error(fetch_type, NULL, "Cannot access static:: when no class scope is active");
				return NULL;
			}
			return ce;
		case ZEND_FETCH_CLASS_AUTO: {
				fetch_sub_type = zend_get_class_fetch_type(class_name);
				if (UNEXPECTED(fetch_sub_type != ZEND_FETCH_CLASS_DEFAULT)) {
					goto check_fetch_type;
				}
			}
			break;
	}

	if (fetch_type & ZEND_FETCH_CLASS_NO_AUTOLOAD) {
		return zend_lookup_class_ex(class_name, NULL, 0);
	} else if ((ce = zend_lookup_class_ex(class_name, NULL, 1)) == NULL) {
		if (!(fetch_type & ZEND_FETCH_CLASS_SILENT) && !EG(exception)) {
			if (fetch_sub_type == ZEND_FETCH_CLASS_INTERFACE) {
				zend_throw_or_error(fetch_type, NULL, "Interface '%s' not found", ZSTR_VAL(class_name));
			} else if (fetch_sub_type == ZEND_FETCH_CLASS_TRAIT) {
				zend_throw_or_error(fetch_type, NULL, "Trait '%s' not found", ZSTR_VAL(class_name));
			} else {
				zend_throw_or_error(fetch_type, NULL, "Class '%s' not found", ZSTR_VAL(class_name));
			}
		}
		return NULL;
	}
	return ce;
}
/* }}} */
