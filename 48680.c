svcauth_gss_acquire_cred(void)
{
	OM_uint32	maj_stat, min_stat;

	log_debug("in svcauth_gss_acquire_cred()");

	maj_stat = gss_acquire_cred(&min_stat, svcauth_gss_name, 0,
				    GSS_C_NULL_OID_SET, GSS_C_ACCEPT,
				    &svcauth_gss_creds, NULL, NULL);

	if (maj_stat != GSS_S_COMPLETE) {
		log_status("gss_acquire_cred", maj_stat, min_stat);
		return (FALSE);
	}
	return (TRUE);
}
