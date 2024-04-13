php_mysqlnd_read_row_ex(MYSQLND_CONN_DATA * conn, MYSQLND_MEMORY_POOL * result_set_memory_pool,
						MYSQLND_MEMORY_POOL_CHUNK ** buffer,
						size_t * data_size, zend_bool persistent_alloc,
						unsigned int prealloc_more_bytes TSRMLS_DC)
{
	enum_func_status ret = PASS;
	MYSQLND_PACKET_HEADER header;
	zend_uchar * p = NULL;
	zend_bool first_iteration = TRUE;

	DBG_ENTER("php_mysqlnd_read_row_ex");

	/*
	  To ease the process the server splits everything in packets up to 2^24 - 1.
	  Even in the case the payload is evenly divisible by this value, the last
	  packet will be empty, namely 0 bytes. Thus, we can read every packet and ask
	  for next one if they have 2^24 - 1 sizes. But just read the header of a
	  zero-length byte, don't read the body, there is no such.
	*/

	*data_size = prealloc_more_bytes;
	while (1) {
		if (FAIL == mysqlnd_read_header(conn->net, &header, conn->stats, conn->error_info TSRMLS_CC)) {
			ret = FAIL;
			break;
		}

		*data_size += header.size;

		if (first_iteration) {
			first_iteration = FALSE;
			/*
			  We need a trailing \0 for the last string, in case of text-mode,
			  to be able to implement read-only variables. Thus, we add + 1.
			*/
			*buffer = result_set_memory_pool->get_chunk(result_set_memory_pool, *data_size + 1 TSRMLS_CC);
			if (!*buffer) {
				ret = FAIL;
				break;
			}
			p = (*buffer)->ptr;
		} else if (!first_iteration) {
			/* Empty packet after MYSQLND_MAX_PACKET_SIZE packet. That's ok, break */
			if (!header.size) {
				break;
			}

			/*
			  We have to realloc the buffer.

			  We need a trailing \0 for the last string, in case of text-mode,
			  to be able to implement read-only variables.
			*/
			if (FAIL == (*buffer)->resize_chunk((*buffer), *data_size + 1 TSRMLS_CC)) {
				SET_OOM_ERROR(*conn->error_info);
				ret = FAIL;
				break;
			}
			/* The position could have changed, recalculate */
			p = (*buffer)->ptr + (*data_size - header.size);
		}

		if (PASS != (ret = conn->net->data->m.receive_ex(conn->net, p, header.size, conn->stats, conn->error_info TSRMLS_CC))) {
			DBG_ERR("Empty row packet body");
			php_error(E_WARNING, "Empty row packet body");
			break;
		}

		if (header.size < MYSQLND_MAX_PACKET_SIZE) {
			break;
		}
	}
	if (ret == FAIL && *buffer) {
		(*buffer)->free_chunk((*buffer) TSRMLS_CC);
		*buffer = NULL;
	}
	*data_size -= prealloc_more_bytes;
	DBG_RETURN(ret);
}
