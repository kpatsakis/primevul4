init_ctx_cont(OM_uint32 *minor_status, gss_ctx_id_t *ctx, gss_buffer_t buf,
	      gss_buffer_t *responseToken, gss_buffer_t *mechListMIC,
	      OM_uint32 *negState, send_token_flag *tokflag)
{
	OM_uint32 ret, tmpmin, acc_negState;
	unsigned char *ptr;
	spnego_gss_ctx_id_t sc;
	gss_OID supportedMech = GSS_C_NO_OID;

	sc = (spnego_gss_ctx_id_t)*ctx;
	*negState = REJECT;
	*tokflag = ERROR_TOKEN_SEND;

	ptr = buf->value;
	ret = get_negTokenResp(minor_status, ptr, buf->length,
			       &acc_negState, &supportedMech,
			       responseToken, mechListMIC);
	if (ret != GSS_S_COMPLETE)
		goto cleanup;
	if (acc_negState == REJECT) {
		*minor_status = ERR_SPNEGO_NEGOTIATION_FAILED;
		map_errcode(minor_status);
		*tokflag = NO_TOKEN_SEND;
		ret = GSS_S_FAILURE;
		goto cleanup;
	}
	/*
	 * nego_done is false for the first call to init_ctx_cont()
	 */
	if (!sc->nego_done) {
		ret = init_ctx_nego(minor_status, sc,
				    acc_negState,
				    supportedMech, responseToken,
				    mechListMIC,
				    negState, tokflag);
	} else if ((!sc->mech_complete && *responseToken == GSS_C_NO_BUFFER) ||
		   (sc->mech_complete && *responseToken != GSS_C_NO_BUFFER)) {
		/* Missing or spurious token from acceptor. */
		ret = GSS_S_DEFECTIVE_TOKEN;
	} else if (!sc->mech_complete ||
		   (sc->mic_reqd &&
		    (sc->ctx_flags & GSS_C_INTEG_FLAG))) {
		/* Not obviously done; we may decide we're done later in
		 * init_ctx_call_init or handle_mic. */
		*negState = ACCEPT_INCOMPLETE;
		*tokflag = CONT_TOKEN_SEND;
		ret = GSS_S_CONTINUE_NEEDED;
	} else {
		/* mech finished on last pass and no MIC required, so done. */
		*negState = ACCEPT_COMPLETE;
		*tokflag = NO_TOKEN_SEND;
		ret = GSS_S_COMPLETE;
	}
cleanup:
	if (supportedMech != GSS_C_NO_OID)
		generic_gss_release_oid(&tmpmin, &supportedMech);
	return ret;
}
