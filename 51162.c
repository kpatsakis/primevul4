php_mysqlnd_prepare_read(void * _packet, MYSQLND_CONN_DATA * conn TSRMLS_DC)
{
	/* In case of an error, we should have place to put it */
	size_t buf_len = conn->net->cmd_buffer.length;
	zend_uchar *buf = (zend_uchar *) conn->net->cmd_buffer.buffer;
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	unsigned int data_size;
	MYSQLND_PACKET_PREPARE_RESPONSE *packet= (MYSQLND_PACKET_PREPARE_RESPONSE *) _packet;

	DBG_ENTER("php_mysqlnd_prepare_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_len, "prepare", PROT_PREPARE_RESP_PACKET);
	BAIL_IF_NO_MORE_DATA;

	data_size = packet->header.size;
	packet->error_code = uint1korr(p);
	p++;
	BAIL_IF_NO_MORE_DATA;

	if (ERROR_MARKER == packet->error_code) {
		php_mysqlnd_read_error_from_line(p, data_size - 1,
										 packet->error_info.error,
										 sizeof(packet->error_info.error),
										 &packet->error_info.error_no,
										 packet->error_info.sqlstate
										 TSRMLS_CC);
		DBG_RETURN(PASS);
	}

	if (data_size != PREPARE_RESPONSE_SIZE_41 &&
		data_size != PREPARE_RESPONSE_SIZE_50 &&
		!(data_size > PREPARE_RESPONSE_SIZE_50)) {
		DBG_ERR_FMT("Wrong COM_STMT_PREPARE response size. Received %u", data_size);
		php_error(E_WARNING, "Wrong COM_STMT_PREPARE response size. Received %u", data_size);
		DBG_RETURN(FAIL);
	}

	packet->stmt_id = uint4korr(p);
	p += 4;
	BAIL_IF_NO_MORE_DATA;

	/* Number of columns in result set */
	packet->field_count = uint2korr(p);
	p += 2;
	BAIL_IF_NO_MORE_DATA;

	packet->param_count = uint2korr(p);
	p += 2;
	BAIL_IF_NO_MORE_DATA;

	if (data_size > 9) {
		/* 0x0 filler sent by the server for 5.0+ clients */
		p++;
		BAIL_IF_NO_MORE_DATA;

		packet->warning_count = uint2korr(p);
	}

	DBG_INF_FMT("Prepare packet read: stmt_id=%u fields=%u params=%u",
				packet->stmt_id, packet->field_count, packet->param_count);

	BAIL_IF_NO_MORE_DATA;

	DBG_RETURN(PASS);
premature_end:
	DBG_ERR_FMT("PREPARE packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "PREPARE packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
