php_mysqlnd_read_error_from_line(zend_uchar *buf, size_t buf_len,
								char *error, int error_buf_len,
								unsigned int *error_no, char *sqlstate TSRMLS_DC)
{
	zend_uchar *p = buf;
	int error_msg_len= 0;

	DBG_ENTER("php_mysqlnd_read_error_from_line");

	*error_no = CR_UNKNOWN_ERROR;
	memcpy(sqlstate, unknown_sqlstate, MYSQLND_SQLSTATE_LENGTH);

	if (buf_len > 2) {
		*error_no = uint2korr(p);
		p+= 2;
		/*
		  sqlstate is following. No need to check for buf_left_len as we checked > 2 above,
		  if it was >=2 then we would need a check
		*/
		if (*p == '#') {
			++p;
			if ((buf_len - (p - buf)) >= MYSQLND_SQLSTATE_LENGTH) {
				memcpy(sqlstate, p, MYSQLND_SQLSTATE_LENGTH);
				p+= MYSQLND_SQLSTATE_LENGTH;
			} else {
				goto end;
			}
		}
		if ((buf_len - (p - buf)) > 0) {
			error_msg_len = MIN((int)((buf_len - (p - buf))), (int) (error_buf_len - 1));
			memcpy(error, p, error_msg_len);
		}
	}
end:
	sqlstate[MYSQLND_SQLSTATE_LENGTH] = '\0';
	error[error_msg_len]= '\0';

	DBG_RETURN(FAIL);
}
