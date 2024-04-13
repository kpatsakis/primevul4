static zend_object *spl_dllist_object_new(zend_class_entry *class_type) /* {{{ */
{
	return spl_dllist_object_new_ex(class_type, NULL, 0);
}
/* }}} */
