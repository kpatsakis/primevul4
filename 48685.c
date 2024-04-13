void svcauth_gss_set_log_badauth_func(
	auth_gssapi_log_badauth_func func,
	caddr_t data)
{
	log_badauth = func;
	log_badauth_data = data;
}
