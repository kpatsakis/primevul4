void php_mysqlnd_rset_field_free_mem(void * _packet, zend_bool stack_allocation TSRMLS_DC)
{
	MYSQLND_PACKET_RES_FIELD *p= (MYSQLND_PACKET_RES_FIELD *) _packet;
	/* p->metadata was passed to us as temporal buffer */
	if (!stack_allocation) {
		mnd_pefree(p, p->header.persistent);
	}
}
