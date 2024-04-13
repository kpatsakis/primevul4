mysqlnd_protocol_free(MYSQLND_PROTOCOL * const protocol TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_protocol_free");

	if (protocol) {
		zend_bool pers = protocol->persistent;
		mnd_pefree(protocol, pers);
	}
	DBG_VOID_RETURN;
}
