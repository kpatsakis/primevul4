mech_requires_mechlistMIC(spnego_gss_ctx_id_t sc)
{
	OM_uint32 major, minor;
	gss_ctx_id_t ctx = sc->ctx_handle;
	gss_OID oid = (gss_OID)&spnego_req_mechlistMIC_oid;
	gss_buffer_set_t bufs;
	int result;

	major = gss_inquire_sec_context_by_oid(&minor, ctx, oid, &bufs);
	if (major != GSS_S_COMPLETE)
		return 0;

	/* Report true if the mech returns a single buffer containing a single
	 * byte with value 1. */
	result = (bufs != NULL && bufs->count == 1 &&
		  bufs->elements[0].length == 1 &&
		  memcmp(bufs->elements[0].value, "\1", 1) == 0);
	(void) gss_release_buffer_set(&minor, &bufs);
	return result;
}
