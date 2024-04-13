php_mysqlnd_greet_read(void * _packet, MYSQLND_CONN_DATA * conn TSRMLS_DC)
{
	zend_uchar buf[2048];
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	zend_uchar *pad_start = NULL;
	MYSQLND_PACKET_GREET *packet= (MYSQLND_PACKET_GREET *) _packet;

	DBG_ENTER("php_mysqlnd_greet_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, sizeof(buf), "greeting", PROT_GREET_PACKET);
	BAIL_IF_NO_MORE_DATA;

	packet->auth_plugin_data = packet->intern_auth_plugin_data;
	packet->auth_plugin_data_len = sizeof(packet->intern_auth_plugin_data);

	if (packet->header.size < sizeof(buf)) {
		/*
		  Null-terminate the string, so strdup can work even if the packets have a string at the end,
		  which is not ASCIIZ
		*/
		buf[packet->header.size] = '\0'; 
	}

	packet->protocol_version = uint1korr(p);
	p++;
	BAIL_IF_NO_MORE_DATA;

	if (ERROR_MARKER == packet->protocol_version) {
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error, sizeof(packet->error),
										 &packet->error_no, packet->sqlstate
										 TSRMLS_CC);
		/*
		  The server doesn't send sqlstate in the greet packet.
		  It's a bug#26426 , so we have to set it correctly ourselves.
		  It's probably "Too many connections, which has SQL state 08004".
		*/
		if (packet->error_no == 1040) {
			memcpy(packet->sqlstate, "08004", MYSQLND_SQLSTATE_LENGTH);
		}
		DBG_RETURN(PASS);
	}

	packet->server_version = estrdup((char *)p);
	p+= strlen(packet->server_version) + 1; /* eat the '\0' */
	BAIL_IF_NO_MORE_DATA;

	packet->thread_id = uint4korr(p);
	p+=4;
	BAIL_IF_NO_MORE_DATA;

	memcpy(packet->auth_plugin_data, p, SCRAMBLE_LENGTH_323);
	p+= SCRAMBLE_LENGTH_323;
	BAIL_IF_NO_MORE_DATA;

	/* pad1 */
	p++;
	BAIL_IF_NO_MORE_DATA;

	packet->server_capabilities = uint2korr(p);
	p+= 2;
	BAIL_IF_NO_MORE_DATA;

	packet->charset_no = uint1korr(p);
	p++;
	BAIL_IF_NO_MORE_DATA;

	packet->server_status = uint2korr(p);
	p+= 2;
	BAIL_IF_NO_MORE_DATA;

	/* pad2 */
	pad_start = p;
	p+= 13;
	BAIL_IF_NO_MORE_DATA;

	if ((size_t) (p - buf) < packet->header.size) {
		/* auth_plugin_data is split into two parts */
		memcpy(packet->auth_plugin_data + SCRAMBLE_LENGTH_323, p, SCRAMBLE_LENGTH - SCRAMBLE_LENGTH_323);
		p+= SCRAMBLE_LENGTH - SCRAMBLE_LENGTH_323;
		p++; /* 0x0 at the end of the scramble and thus last byte in the packet in 5.1 and previous */
	} else {
		packet->pre41 = TRUE;
	}

	/* Is this a 5.5+ server ? */
	if ((size_t) (p - buf) < packet->header.size) {
		 /* backtrack one byte, the 0x0 at the end of the scramble in 5.1 and previous */
		p--;

    	/* Additional 16 bits for server capabilities */
		packet->server_capabilities |= uint2korr(pad_start) << 16;
		/* And a length of the server scramble in one byte */
		packet->auth_plugin_data_len = uint1korr(pad_start + 2);
		if (packet->auth_plugin_data_len > SCRAMBLE_LENGTH) {
			/* more data*/
			zend_uchar * new_auth_plugin_data = emalloc(packet->auth_plugin_data_len);
			if (!new_auth_plugin_data) {
				goto premature_end;
			}
			/* copy what we already have */
			memcpy(new_auth_plugin_data, packet->auth_plugin_data, SCRAMBLE_LENGTH);
			/* add additional scramble data 5.5+ sent us */
			memcpy(new_auth_plugin_data + SCRAMBLE_LENGTH, p, packet->auth_plugin_data_len - SCRAMBLE_LENGTH);
			p+= (packet->auth_plugin_data_len - SCRAMBLE_LENGTH);
			packet->auth_plugin_data = new_auth_plugin_data;
		}
	}

	if (packet->server_capabilities & CLIENT_PLUGIN_AUTH) {
		BAIL_IF_NO_MORE_DATA;
		/* The server is 5.5.x and supports authentication plugins */
		packet->auth_protocol = estrdup((char *)p);
		p+= strlen(packet->auth_protocol) + 1; /* eat the '\0' */
	}

	DBG_INF_FMT("proto=%u server=%s thread_id=%u",
				packet->protocol_version, packet->server_version, packet->thread_id);

	DBG_INF_FMT("server_capabilities=%u charset_no=%u server_status=%i auth_protocol=%s scramble_length=%u",
				packet->server_capabilities, packet->charset_no, packet->server_status,
				packet->auth_protocol? packet->auth_protocol:"n/a", packet->auth_plugin_data_len);

	DBG_RETURN(PASS);
premature_end:
	DBG_ERR_FMT("GREET packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "GREET packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
