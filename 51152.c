void php_mysqlnd_eof_free_mem(void * _packet, zend_bool stack_allocation TSRMLS_DC)
{
	if (!stack_allocation) {
		mnd_pefree(_packet, ((MYSQLND_PACKET_EOF *)_packet)->header.persistent);
	}
}
