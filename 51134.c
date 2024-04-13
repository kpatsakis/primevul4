MYSQLND_METHOD(mysqlnd_protocol, get_result_field_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC)
{
	struct st_mysqlnd_packet_res_field * packet = mnd_pecalloc(1, packet_methods[PROT_RSET_FLD_PACKET].struct_size, persistent);
	DBG_ENTER("mysqlnd_protocol::get_result_field_packet");
	if (packet) {
		packet->header.m = &packet_methods[PROT_RSET_FLD_PACKET];
		packet->header.persistent = persistent;
	}
	DBG_RETURN(packet);
}
