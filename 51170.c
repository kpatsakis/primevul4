php_mysqlnd_rset_field_read(void * _packet, MYSQLND_CONN_DATA * conn TSRMLS_DC)
{
	/* Should be enough for the metadata of a single row */
	MYSQLND_PACKET_RES_FIELD *packet= (MYSQLND_PACKET_RES_FIELD *) _packet;
	size_t buf_len = conn->net->cmd_buffer.length, total_len = 0;
	zend_uchar *buf = (zend_uchar *) conn->net->cmd_buffer.buffer;
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	char *root_ptr;
	unsigned long len;
	MYSQLND_FIELD *meta;
	unsigned int i, field_count = sizeof(rset_field_offsets)/sizeof(size_t);

	DBG_ENTER("php_mysqlnd_rset_field_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_len, "field", PROT_RSET_FLD_PACKET);

	if (packet->skip_parsing) {
		DBG_RETURN(PASS);
	}

	BAIL_IF_NO_MORE_DATA;
	if (ERROR_MARKER == *p) {
		/* Error */
		p++;
		BAIL_IF_NO_MORE_DATA;
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error_info.error, sizeof(packet->error_info.error),
										 &packet->error_info.error_no, packet->error_info.sqlstate
										 TSRMLS_CC);
		DBG_ERR_FMT("Server error : (%u) %s", packet->error_info.error_no, packet->error_info.error);
		DBG_RETURN(PASS);
	} else if (EODATA_MARKER == *p && packet->header.size < 8) {
		/* Premature EOF. That should be COM_FIELD_LIST */
		DBG_INF("Premature EOF. That should be COM_FIELD_LIST");
		packet->stupid_list_fields_eof = TRUE;
		DBG_RETURN(PASS);
	}

	meta = packet->metadata;

	for (i = 0; i < field_count; i += 2) {
		len = php_mysqlnd_net_field_length(&p);
		BAIL_IF_NO_MORE_DATA;
		switch ((len)) {
			case 0:
				*(const char **)(((char*)meta) + rset_field_offsets[i]) = mysqlnd_empty_string;
				*(unsigned int *)(((char*)meta) + rset_field_offsets[i+1]) = 0;
				break;
			case MYSQLND_NULL_LENGTH:
				goto faulty_or_fake;
			default:
				*(const char **)(((char *)meta) + rset_field_offsets[i]) = (const char *)p;
				*(unsigned int *)(((char*)meta) + rset_field_offsets[i+1]) = len;
				p += len;
				total_len += len + 1;
				break;
		}
		BAIL_IF_NO_MORE_DATA;
	}

	/* 1 byte length */
	if (12 != *p) {
		DBG_ERR_FMT("Protocol error. Server sent false length. Expected 12 got %d", (int) *p);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Protocol error. Server sent false length. Expected 12");
	}
	p++;
	BAIL_IF_NO_MORE_DATA;

	meta->charsetnr = uint2korr(p);
	p += 2;
	BAIL_IF_NO_MORE_DATA;

	meta->length = uint4korr(p);
	p += 4;
	BAIL_IF_NO_MORE_DATA;

	meta->type = uint1korr(p);
	p += 1;
	BAIL_IF_NO_MORE_DATA;

	meta->flags = uint2korr(p);
	p += 2;
	BAIL_IF_NO_MORE_DATA;

	meta->decimals = uint1korr(p);
	p += 1;
	BAIL_IF_NO_MORE_DATA;

	/* 2 byte filler */
	p +=2;
	BAIL_IF_NO_MORE_DATA;

	/* Should we set NUM_FLAG (libmysql does it) ? */
	if (
		(meta->type <= MYSQL_TYPE_INT24 &&
			(meta->type != MYSQL_TYPE_TIMESTAMP || meta->length == 14 || meta->length == 8)
		) || meta->type == MYSQL_TYPE_YEAR)
	{
		meta->flags |= NUM_FLAG;
	}


	/*
	  def could be empty, thus don't allocate on the root.
	  NULL_LENGTH (0xFB) comes from COM_FIELD_LIST when the default value is NULL.
	  Otherwise the string is length encoded.
	*/
	if (packet->header.size > (size_t) (p - buf) &&
		(len = php_mysqlnd_net_field_length(&p)) &&
		len != MYSQLND_NULL_LENGTH)
	{
		BAIL_IF_NO_MORE_DATA;
		DBG_INF_FMT("Def found, length %lu, persistent=%u", len, packet->persistent_alloc);
		meta->def = mnd_pemalloc(len + 1, packet->persistent_alloc);
		if (!meta->def) {
			SET_OOM_ERROR(*conn->error_info);
			DBG_RETURN(FAIL);		
		}
		memcpy(meta->def, p, len);
		meta->def[len] = '\0';
		meta->def_length = len;
		p += len;
	}

	DBG_INF_FMT("allocing root. persistent=%u", packet->persistent_alloc);
	root_ptr = meta->root = mnd_pemalloc(total_len, packet->persistent_alloc);
	if (!root_ptr) {
		SET_OOM_ERROR(*conn->error_info);
		DBG_RETURN(FAIL);	
	}
	
	meta->root_len = total_len;
	/* Now do allocs */
	if (meta->catalog && meta->catalog != mysqlnd_empty_string) {
		len = meta->catalog_length;
		meta->catalog = memcpy(root_ptr, meta->catalog, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->db && meta->db != mysqlnd_empty_string) {
		len = meta->db_length;
		meta->db = memcpy(root_ptr, meta->db, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->table && meta->table != mysqlnd_empty_string) {
		len = meta->table_length;
		meta->table = memcpy(root_ptr, meta->table, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->org_table && meta->org_table != mysqlnd_empty_string) {
		len = meta->org_table_length;
		meta->org_table = memcpy(root_ptr, meta->org_table, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->name && meta->name != mysqlnd_empty_string) {
		len = meta->name_length;
		meta->name = memcpy(root_ptr, meta->name, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->org_name && meta->org_name != mysqlnd_empty_string) {
		len = meta->org_name_length;
		meta->org_name = memcpy(root_ptr, meta->org_name, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	DBG_INF_FMT("FIELD=[%s.%s.%s]", meta->db? meta->db:"*NA*", meta->table? meta->table:"*NA*",
				meta->name? meta->name:"*NA*");

	DBG_RETURN(PASS);

faulty_or_fake:
	DBG_ERR_FMT("Protocol error. Server sent NULL_LENGTH. The server is faulty");
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Protocol error. Server sent NULL_LENGTH."
					 " The server is faulty");
	DBG_RETURN(FAIL);
premature_end:
	DBG_ERR_FMT("RSET field packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Result set field packet "MYSQLND_SZ_T_SPEC" bytes "
			 		"shorter than expected", p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
