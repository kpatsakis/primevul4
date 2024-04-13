size_t php_mysqlnd_cmd_write(void * _packet, MYSQLND_CONN_DATA * conn TSRMLS_DC)
{
	/* Let's have some space, which we can use, if not enough, we will allocate new buffer */
	MYSQLND_PACKET_COMMAND * packet= (MYSQLND_PACKET_COMMAND *) _packet;
	MYSQLND_NET * net = conn->net;
	unsigned int error_reporting = EG(error_reporting);
	size_t sent = 0;

	DBG_ENTER("php_mysqlnd_cmd_write");
	/*
	  Reset packet_no, or we will get bad handshake!
	  Every command starts a new TX and packet numbers are reset to 0.
	*/
	net->packet_no = 0;
	net->compressed_envelope_packet_no = 0; /* this is for the response */

	if (error_reporting) {
		EG(error_reporting) = 0;
	}

	MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_PACKETS_SENT_CMD);

#ifdef MYSQLND_DO_WIRE_CHECK_BEFORE_COMMAND
	net->data->m.consume_uneaten_data(net, packet->command TSRMLS_CC);
#endif

	if (!packet->argument || !packet->arg_len) {
		zend_uchar buffer[MYSQLND_HEADER_SIZE + 1];

		int1store(buffer + MYSQLND_HEADER_SIZE, packet->command);
		sent = net->data->m.send_ex(net, buffer, 1, conn->stats, conn->error_info TSRMLS_CC);
	} else {
		size_t tmp_len = packet->arg_len + 1 + MYSQLND_HEADER_SIZE;
		zend_uchar *tmp, *p;
		tmp = (tmp_len > net->cmd_buffer.length)? mnd_emalloc(tmp_len):net->cmd_buffer.buffer;
		if (!tmp) {
			goto end;
		}
		p = tmp + MYSQLND_HEADER_SIZE; /* skip the header */

		int1store(p, packet->command);
		p++;

		memcpy(p, packet->argument, packet->arg_len);

		sent = net->data->m.send_ex(net, tmp, tmp_len - MYSQLND_HEADER_SIZE, conn->stats, conn->error_info TSRMLS_CC);
		if (tmp != net->cmd_buffer.buffer) {
			MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_CMD_BUFFER_TOO_SMALL);
			mnd_efree(tmp);
		}
	}
end:
	if (error_reporting) {
		/* restore error reporting */
		EG(error_reporting) = error_reporting;
	}
	if (!sent) {
		CONN_SET_STATE(conn, CONN_QUIT_SENT);
	}
	DBG_RETURN(sent);
}
