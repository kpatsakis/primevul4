php_mysqlnd_chg_user_free_mem(void * _packet, zend_bool stack_allocation TSRMLS_DC)
{
	MYSQLND_PACKET_CHG_USER_RESPONSE * p = (MYSQLND_PACKET_CHG_USER_RESPONSE *) _packet;

	if (p->new_auth_protocol) {
		mnd_efree(p->new_auth_protocol);
		p->new_auth_protocol = NULL;
	}
	p->new_auth_protocol_len = 0;

	if (p->new_auth_protocol_data) {
		mnd_efree(p->new_auth_protocol_data);
		p->new_auth_protocol_data = NULL;
	}
	p->new_auth_protocol_data_len = 0;

	if (!stack_allocation) {
		mnd_pefree(p, p->header.persistent);
	}
}
