void svcauth_gss_set_log_badverf_func(
	auth_gssapi_log_badverf_func func,
	caddr_t data)
{
	log_badverf = func;
	log_badverf_data = data;
}
