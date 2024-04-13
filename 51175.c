void php_mysqlnd_stats_free_mem(void * _packet, zend_bool stack_allocation TSRMLS_DC)
{
	MYSQLND_PACKET_STATS *p= (MYSQLND_PACKET_STATS *) _packet;
	if (p->message) {
		mnd_efree(p->message);
		p->message = NULL;
	}
	if (!stack_allocation) {
		mnd_pefree(p, p->header.persistent);
	}
}
