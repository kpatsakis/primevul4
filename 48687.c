void svcauth_gss_set_log_miscerr_func(
	auth_gssapi_log_miscerr_func func,
	caddr_t data)
{
	log_miscerr = func;
	log_miscerr_data = data;
}
