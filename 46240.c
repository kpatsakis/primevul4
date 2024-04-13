__rds_conn_error(struct rds_connection *conn, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vprintk(fmt, ap);
	va_end(ap);

	rds_conn_drop(conn);
}
