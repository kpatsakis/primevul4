__SSL_critical_error (char *funcname)
{
	__SSL_fill_err_buf (funcname);
	fprintf (stderr, "%s\n", err_buf);

	exit (1);
}
