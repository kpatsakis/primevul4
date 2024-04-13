static inline spl_dllist_object *spl_dllist_from_obj(zend_object *obj) /* {{{ */ {
	return (spl_dllist_object*)((char*)(obj) - XtOffsetOf(spl_dllist_object, std));
}
/* }}} */
