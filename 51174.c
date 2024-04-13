php_mysqlnd_sha256_pk_request_response_read(void * _packet, MYSQLND_CONN_DATA * conn TSRMLS_DC)
{
	zend_uchar buf[SHA256_PK_REQUEST_RESP_BUFFER_SIZE];
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	MYSQLND_PACKET_SHA256_PK_REQUEST_RESPONSE * packet= (MYSQLND_PACKET_SHA256_PK_REQUEST_RESPONSE *) _packet;

	DBG_ENTER("php_mysqlnd_sha256_pk_request_response_read");

	/* leave space for terminating safety \0 */
	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, sizeof(buf), "SHA256_PK_REQUEST_RESPONSE", PROT_SHA256_PK_REQUEST_RESPONSE_PACKET);
	BAIL_IF_NO_MORE_DATA;

	p++;
	BAIL_IF_NO_MORE_DATA;

	packet->public_key_len = packet->header.size - (p - buf);
	packet->public_key = mnd_emalloc(packet->public_key_len + 1);
	memcpy(packet->public_key, p, packet->public_key_len);
	packet->public_key[packet->public_key_len] = '\0';

	DBG_RETURN(PASS);

premature_end:
	DBG_ERR_FMT("OK packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "SHA256_PK_REQUEST_RESPONSE packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
