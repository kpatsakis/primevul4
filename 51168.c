php_mysqlnd_rowp_read_text_protocol_zval(MYSQLND_MEMORY_POOL_CHUNK * row_buffer, zval ** fields,
									unsigned int field_count, const MYSQLND_FIELD * fields_metadata,
									zend_bool as_int_or_float, MYSQLND_STATS * stats TSRMLS_DC)
{
	enum_func_status ret;
	DBG_ENTER("php_mysqlnd_rowp_read_text_protocol_zval");
	ret = php_mysqlnd_rowp_read_text_protocol_aux(row_buffer, fields, field_count, fields_metadata, as_int_or_float, FALSE, stats TSRMLS_CC);
	DBG_RETURN(ret);
}
