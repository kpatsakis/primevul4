badauth(OM_uint32 maj, OM_uint32 minor, SVCXPRT *xprt)
{
	if (log_badauth != NULL)
		(*log_badauth)(maj, minor, &xprt->xp_raddr, log_badauth_data);
	if (log_badauth2 != NULL)
		(*log_badauth2)(maj, minor, xprt, log_badauth2_data);
}
