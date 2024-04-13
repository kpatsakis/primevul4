mysqlnd_read_header(MYSQLND_NET * net, MYSQLND_PACKET_HEADER * header,
					MYSQLND_STATS * conn_stats, MYSQLND_ERROR_INFO * error_info TSRMLS_DC)
{
	zend_uchar buffer[MYSQLND_HEADER_SIZE];

	DBG_ENTER(mysqlnd_read_header_name);
	DBG_INF_FMT("compressed=%u", net->data->compressed);
	if (FAIL == net->data->m.receive_ex(net, buffer, MYSQLND_HEADER_SIZE, conn_stats, error_info TSRMLS_CC)) {
		DBG_RETURN(FAIL);
	}

	header->size = uint3korr(buffer);
	header->packet_no = uint1korr(buffer + 3);

#ifdef MYSQLND_DUMP_HEADER_N_BODY
	DBG_INF_FMT("HEADER: prot_packet_no=%u size=%3u", header->packet_no, header->size);
#endif
	MYSQLND_INC_CONN_STATISTIC_W_VALUE2(conn_stats,
							STAT_PROTOCOL_OVERHEAD_IN, MYSQLND_HEADER_SIZE,
							STAT_PACKETS_RECEIVED, 1);

	if (net->data->compressed || net->packet_no == header->packet_no) {
		/*
		  Have to increase the number, so we can send correct number back. It will
		  round at 255 as this is unsigned char. The server needs this for simple
		  flow control checking.
		*/
		net->packet_no++;
		DBG_RETURN(PASS);
	}

	DBG_ERR_FMT("Logical link: packets out of order. Expected %u received %u. Packet size="MYSQLND_SZ_T_SPEC,
				net->packet_no, header->packet_no, header->size);

	php_error(E_WARNING, "Packets out of order. Expected %u received %u. Packet size="MYSQLND_SZ_T_SPEC,
			  net->packet_no, header->packet_no, header->size);
	DBG_RETURN(FAIL);
}
