php_mysqlnd_chg_user_read(void * _packet, MYSQLND_CONN_DATA * conn TSRMLS_DC)
{
	/* There could be an error message */
	size_t buf_len = conn->net->cmd_buffer.length;
	zend_uchar *buf = (zend_uchar *) conn->net->cmd_buffer.buffer;
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	MYSQLND_PACKET_CHG_USER_RESPONSE *packet= (MYSQLND_PACKET_CHG_USER_RESPONSE *) _packet;

	DBG_ENTER("php_mysqlnd_chg_user_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_len, "change user response", PROT_CHG_USER_RESP_PACKET);
	BAIL_IF_NO_MORE_DATA;

	/*
	  Don't increment. First byte is ERROR_MARKER on error, but otherwise is starting byte
	  of encoded sequence for length.
	*/

	/* Should be always 0x0 or ERROR_MARKER for error */
	packet->response_code = uint1korr(p);
	p++;

	if (packet->header.size == 1 && buf[0] == EODATA_MARKER && packet->server_capabilities & CLIENT_SECURE_CONNECTION) {
		/* We don't handle 3.23 authentication */
		packet->server_asked_323_auth = TRUE;
		DBG_RETURN(FAIL);
	}

	if (ERROR_MARKER == packet->response_code) {
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error_info.error,
										 sizeof(packet->error_info.error),
										 &packet->error_info.error_no,
										 packet->error_info.sqlstate
										 TSRMLS_CC);
	}
	BAIL_IF_NO_MORE_DATA;
	if (packet->response_code == 0xFE && packet->header.size > (size_t) (p - buf)) {
		packet->new_auth_protocol = mnd_pestrdup((char *)p, FALSE);
		packet->new_auth_protocol_len = strlen(packet->new_auth_protocol);
		p+= packet->new_auth_protocol_len + 1; /* +1 for the \0 */
		packet->new_auth_protocol_data_len = packet->header.size - (size_t) (p - buf);
		if (packet->new_auth_protocol_data_len) {
			packet->new_auth_protocol_data = mnd_emalloc(packet->new_auth_protocol_data_len);
			memcpy(packet->new_auth_protocol_data, p, packet->new_auth_protocol_data_len);
		}
		DBG_INF_FMT("The server requested switching auth plugin to : %s", packet->new_auth_protocol);
		DBG_INF_FMT("Server salt : [%*s]", packet->new_auth_protocol_data_len, packet->new_auth_protocol_data);
	}

	DBG_RETURN(PASS);
premature_end:
	DBG_ERR_FMT("CHANGE_USER packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "CHANGE_USER packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
						 p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
