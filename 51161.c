php_mysqlnd_prepare_free_mem(void * _packet, zend_bool stack_allocation TSRMLS_DC)
{
	MYSQLND_PACKET_PREPARE_RESPONSE *p= (MYSQLND_PACKET_PREPARE_RESPONSE *) _packet;
	if (!stack_allocation) {
		mnd_pefree(p, p->header.persistent);
	}
}
