void php_mysqlnd_cmd_free_mem(void * _packet, zend_bool stack_allocation TSRMLS_DC)
{
	if (!stack_allocation) {
		MYSQLND_PACKET_COMMAND * p = (MYSQLND_PACKET_COMMAND *) _packet;
		mnd_pefree(p, p->header.persistent);
	}
}
