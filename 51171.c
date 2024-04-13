void php_mysqlnd_rset_header_free_mem(void * _packet, zend_bool stack_allocation TSRMLS_DC)
{
	MYSQLND_PACKET_RSET_HEADER *p= (MYSQLND_PACKET_RSET_HEADER *) _packet;
	DBG_ENTER("php_mysqlnd_rset_header_free_mem");
	if (p->info_or_local_file) {
		mnd_efree(p->info_or_local_file);
		p->info_or_local_file = NULL;
	}
	if (!stack_allocation) {
		mnd_pefree(p, p->header.persistent);
	}
	DBG_VOID_RETURN;
}
