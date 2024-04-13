void php_mysqlnd_sha256_pk_request_free_mem(void * _packet, zend_bool stack_allocation TSRMLS_DC)
{
	if (!stack_allocation) {
		MYSQLND_PACKET_SHA256_PK_REQUEST * p = (MYSQLND_PACKET_SHA256_PK_REQUEST *) _packet;
		mnd_pefree(p, p->header.persistent);
	}
}
