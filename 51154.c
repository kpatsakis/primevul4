void php_mysqlnd_greet_free_mem(void * _packet, zend_bool stack_allocation TSRMLS_DC)
{
	MYSQLND_PACKET_GREET *p= (MYSQLND_PACKET_GREET *) _packet;
	if (p->server_version) {
		efree(p->server_version);
		p->server_version = NULL;
	}
	if (p->auth_plugin_data && p->auth_plugin_data != p->intern_auth_plugin_data) {
		efree(p->auth_plugin_data);
		p->auth_plugin_data = NULL;
	}
	if (p->auth_protocol) {
		efree(p->auth_protocol);
		p->auth_protocol = NULL;
	}
	if (!stack_allocation) {
		mnd_pefree(p, p->header.persistent);
	}
}
