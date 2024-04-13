MYSQLND_METHOD(mysqlnd_protocol, get_eof_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC)
{
	struct st_mysqlnd_packet_eof * packet = mnd_pecalloc(1, packet_methods[PROT_EOF_PACKET].struct_size, persistent);
	DBG_ENTER("mysqlnd_protocol::get_eof_packet");
	if (packet) {
		packet->header.m = &packet_methods[PROT_EOF_PACKET];
		packet->header.persistent = persistent;
	}
	DBG_RETURN(packet);
}
