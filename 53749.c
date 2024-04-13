static void zend_extension_activator(zend_extension *extension) /* {{{ */
{
	if (extension->activate) {
		extension->activate();
	}
}
/* }}} */
