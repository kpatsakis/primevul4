svcauth_gss_set_log_badauth2_func(auth_gssapi_log_badauth2_func func,
				  caddr_t data)
{
	log_badauth2 = func;
	log_badauth2_data = data;
}
