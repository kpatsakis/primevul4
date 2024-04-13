void php_mysqlnd_auth_free_mem(void * _packet, zend_bool stack_allocation TSRMLS_DC)
{
	if (!stack_allocation) {
		MYSQLND_PACKET_AUTH * p = (MYSQLND_PACKET_AUTH *) _packet;
		mnd_pefree(p, p->header.persistent);
	}
}
