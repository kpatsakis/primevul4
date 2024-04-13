svcauth_gss_release_cred(void)
{
	OM_uint32	maj_stat, min_stat;

	log_debug("in svcauth_gss_release_cred()");

	maj_stat = gss_release_cred(&min_stat, &svcauth_gss_creds);

	if (maj_stat != GSS_S_COMPLETE) {
		log_status("gss_release_cred", maj_stat, min_stat);
		return (FALSE);
	}

	svcauth_gss_creds = NULL;

	return (TRUE);
}
