svcauth_gss_destroy(SVCAUTH *auth)
{
	struct svc_rpc_gss_data	*gd;
	OM_uint32		 min_stat;

	log_debug("in svcauth_gss_destroy()");

	gd = SVCAUTH_PRIVATE(auth);

	gss_delete_sec_context(&min_stat, &gd->ctx, GSS_C_NO_BUFFER);
	gss_release_buffer(&min_stat, &gd->cname);
	gss_release_buffer(&min_stat, &gd->checksum);

	if (gd->client_name)
		gss_release_name(&min_stat, &gd->client_name);

	mem_free(gd, sizeof(*gd));
	mem_free(auth, sizeof(*auth));

	return (TRUE);
}
