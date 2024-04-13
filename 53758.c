ZEND_API zend_class_entry *zend_lookup_class(zend_string *name) /* {{{ */
{
	return zend_lookup_class_ex(name, NULL, 1);
}
/* }}} */
