PHP_FUNCTION(gethostbyname)
{
	char *hostname;
	int hostname_len;
	char *addr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &hostname, &hostname_len) == FAILURE) {
		return;
	}

	addr = php_gethostbyname(hostname);

	RETVAL_STRING(addr, 0);
}
