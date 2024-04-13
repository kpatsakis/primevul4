__SSL_fill_err_buf (char *funcname)
{
	int err;
	char buf[256];


	err = ERR_get_error ();
	ERR_error_string (err, buf);
	g_snprintf (err_buf, sizeof (err_buf), "%s: %s (%d)\n", funcname, buf, err);
}
