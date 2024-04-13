php_mysqlnd_change_auth_response_write(void * _packet, MYSQLND_CONN_DATA * conn TSRMLS_DC)
{
	MYSQLND_PACKET_CHANGE_AUTH_RESPONSE *packet= (MYSQLND_PACKET_CHANGE_AUTH_RESPONSE *) _packet;
	zend_uchar * buffer = conn->net->cmd_buffer.length >= packet->auth_data_len? conn->net->cmd_buffer.buffer : mnd_emalloc(packet->auth_data_len);
	zend_uchar *p = buffer + MYSQLND_HEADER_SIZE; /* start after the header */

	DBG_ENTER("php_mysqlnd_change_auth_response_write");

	if (packet->auth_data_len) {
		memcpy(p, packet->auth_data, packet->auth_data_len);
		p+= packet->auth_data_len;
	}

	{
		size_t sent = conn->net->data->m.send_ex(conn->net, buffer, p - buffer - MYSQLND_HEADER_SIZE, conn->stats, conn->error_info TSRMLS_CC);
		if (buffer != conn->net->cmd_buffer.buffer) {
			mnd_efree(buffer);
		}
		if (!sent) {
			CONN_SET_STATE(conn, CONN_QUIT_SENT);
		}
		DBG_RETURN(sent);
	}
}
