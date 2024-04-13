size_t php_mysqlnd_auth_write(void * _packet, MYSQLND_CONN_DATA * conn TSRMLS_DC)
{
	zend_uchar buffer[AUTH_WRITE_BUFFER_LEN];
	zend_uchar *p = buffer + MYSQLND_HEADER_SIZE; /* start after the header */
	int len;
	MYSQLND_PACKET_AUTH * packet= (MYSQLND_PACKET_AUTH *) _packet;

	DBG_ENTER("php_mysqlnd_auth_write");

	if (!packet->is_change_user_packet) {
		int4store(p, packet->client_flags);
		p+= 4;

		int4store(p, packet->max_packet_size);
		p+= 4;

		int1store(p, packet->charset_no);
		p++;

		memset(p, 0, 23); /* filler */
		p+= 23;
	}

	if (packet->send_auth_data || packet->is_change_user_packet) {
		len = MIN(strlen(packet->user), MYSQLND_MAX_ALLOWED_USER_LEN);
		memcpy(p, packet->user, len);
		p+= len;
		*p++ = '\0';

		/* defensive coding */
		if (packet->auth_data == NULL) {
			packet->auth_data_len = 0;
		}
		if (packet->auth_data_len > 0xFF) {
			const char * const msg = "Authentication data too long. "
				"Won't fit into the buffer and will be truncated. Authentication will thus fail";
			SET_CLIENT_ERROR(*conn->error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, msg);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", msg);
			DBG_RETURN(0);
		}		
		
		int1store(p, packet->auth_data_len);
		++p;
/*!!!!! is the buffer big enough ??? */
		if ((sizeof(buffer) - (p - buffer)) < packet->auth_data_len) {
			DBG_ERR("the stack buffer was not enough!!");
			DBG_RETURN(0);
		}
		if (packet->auth_data_len) {
			memcpy(p, packet->auth_data, packet->auth_data_len);
			p+= packet->auth_data_len;
		}

		if (packet->db) {
			/* CLIENT_CONNECT_WITH_DB should have been set */
			size_t real_db_len = MIN(MYSQLND_MAX_ALLOWED_DB_LEN, packet->db_len);
			memcpy(p, packet->db, real_db_len);
			p+= real_db_len;
			*p++= '\0';
		} else if (packet->is_change_user_packet) {
			*p++= '\0';		
		}
		/* no \0 for no DB */

		if (packet->is_change_user_packet) {
			if (packet->charset_no) {
				int2store(p, packet->charset_no);
				p+= 2;
			}
		}
		
		if (packet->auth_plugin_name) {
			size_t len = MIN(strlen(packet->auth_plugin_name), sizeof(buffer) - (p - buffer) - 1);
			memcpy(p, packet->auth_plugin_name, len);
			p+= len;
			*p++= '\0';
		}

		if (packet->connect_attr && zend_hash_num_elements(packet->connect_attr)) {
			HashPosition pos_value;
			const char ** entry_value;
			size_t ca_payload_len = 0;
			zend_hash_internal_pointer_reset_ex(packet->connect_attr, &pos_value);
			while (SUCCESS == zend_hash_get_current_data_ex(packet->connect_attr, (void **)&entry_value, &pos_value)) {
				char *s_key;
				unsigned int s_len;
				unsigned long num_key;
				size_t value_len = strlen(*entry_value);
				
				if (HASH_KEY_IS_STRING == zend_hash_get_current_key_ex(packet->connect_attr, &s_key, &s_len, &num_key, 0, &pos_value)) {
					ca_payload_len += php_mysqlnd_net_store_length_size(s_len);
					ca_payload_len += s_len;
					ca_payload_len += php_mysqlnd_net_store_length_size(value_len);
					ca_payload_len += value_len;
				}
				zend_hash_move_forward_ex(conn->options->connect_attr, &pos_value);
			}

			if ((sizeof(buffer) - (p - buffer)) >= (ca_payload_len + php_mysqlnd_net_store_length_size(ca_payload_len))) {
				p = php_mysqlnd_net_store_length(p, ca_payload_len);

				zend_hash_internal_pointer_reset_ex(packet->connect_attr, &pos_value);
				while (SUCCESS == zend_hash_get_current_data_ex(packet->connect_attr, (void **)&entry_value, &pos_value)) {
					char *s_key;
					unsigned int s_len;
					unsigned long num_key;
					size_t value_len = strlen(*entry_value);
					if (HASH_KEY_IS_STRING == zend_hash_get_current_key_ex(packet->connect_attr, &s_key, &s_len, &num_key, 0, &pos_value)) {
						/* copy key */
						p = php_mysqlnd_net_store_length(p, s_len);
						memcpy(p, s_key, s_len);
						p+= s_len;
						/* copy value */
						p = php_mysqlnd_net_store_length(p, value_len);
						memcpy(p, *entry_value, value_len);
						p+= value_len;
					}
					zend_hash_move_forward_ex(conn->options->connect_attr, &pos_value);
				}
			} else {
				/* cannot put the data - skip */
			}
		}
	}
	if (packet->is_change_user_packet) {
		if (PASS != conn->m->simple_command(conn, COM_CHANGE_USER, buffer + MYSQLND_HEADER_SIZE, p - buffer - MYSQLND_HEADER_SIZE,
										   PROT_LAST /* the caller will handle the OK packet */,
										   packet->silent, TRUE TSRMLS_CC)) {
			DBG_RETURN(0);
		}
		DBG_RETURN(p - buffer - MYSQLND_HEADER_SIZE);
	} else {
		size_t sent = conn->net->data->m.send_ex(conn->net, buffer, p - buffer - MYSQLND_HEADER_SIZE, conn->stats, conn->error_info TSRMLS_CC);
		if (!sent) {
			CONN_SET_STATE(conn, CONN_QUIT_SENT);
		}
		DBG_RETURN(sent);
	}
}
