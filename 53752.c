zend_class_entry *zend_fetch_class_by_name(zend_string *class_name, const zval *key, int fetch_type) /* {{{ */
{
	zend_class_entry *ce;

	if (fetch_type & ZEND_FETCH_CLASS_NO_AUTOLOAD) {
		return zend_lookup_class_ex(class_name, key, 0);
	} else if ((ce = zend_lookup_class_ex(class_name, key, 1)) == NULL) {
		if ((fetch_type & ZEND_FETCH_CLASS_SILENT) == 0 && !EG(exception)) {
			if ((fetch_type & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_INTERFACE) {
				zend_throw_or_error(fetch_type, NULL, "Interface '%s' not found", ZSTR_VAL(class_name));
			} else if ((fetch_type & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_TRAIT) {
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
