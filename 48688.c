svcauth_gss_set_svc_name(gss_name_t name)
{
	OM_uint32	maj_stat, min_stat;

	log_debug("in svcauth_gss_set_svc_name()");

	if (svcauth_gss_name != NULL) {
		maj_stat = gss_release_name(&min_stat, &svcauth_gss_name);

		if (maj_stat != GSS_S_COMPLETE) {
			log_status("gss_release_name", maj_stat, min_stat);
			return (FALSE);
		}
		svcauth_gss_name = NULL;
	}
	if (svcauth_gss_name == GSS_C_NO_NAME)
		return (TRUE);

	maj_stat = gss_duplicate_name(&min_stat, name, &svcauth_gss_name);

	if (maj_stat != GSS_S_COMPLETE) {
		log_status("gss_duplicate_name", maj_stat, min_stat);
		return (FALSE);
	}

	return (TRUE);
}
