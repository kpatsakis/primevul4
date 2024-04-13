php_mysqlnd_stats_read(void * _packet, MYSQLND_CONN_DATA * conn TSRMLS_DC)
{
	MYSQLND_PACKET_STATS *packet= (MYSQLND_PACKET_STATS *) _packet;
	size_t buf_len = conn->net->cmd_buffer.length;
	zend_uchar *buf = (zend_uchar *) conn->net->cmd_buffer.buffer;

	DBG_ENTER("php_mysqlnd_stats_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_len, "statistics", PROT_STATS_PACKET);

	packet->message = mnd_emalloc(packet->header.size + 1);
	memcpy(packet->message, buf, packet->header.size);
	packet->message[packet->header.size] = '\0';
	packet->message_len = packet->header.size;

	DBG_RETURN(PASS);
}
