vbf_beresp2obj(struct busyobj *bo)
{
	unsigned l, l2;
	const char *b;
	uint8_t *bp;
	struct vsb *vary = NULL;
	int varyl = 0;

	l = 0;

	/* Create Vary instructions */
	if (!(bo->fetch_objcore->flags & OC_F_PRIVATE)) {
		varyl = VRY_Create(bo, &vary);
		if (varyl > 0) {
			AN(vary);
			assert(varyl == VSB_len(vary));
			l += PRNDUP((intptr_t)varyl);
		} else if (varyl < 0) {
			/*
			 * Vary parse error
			 * Complain about it, and make this a pass.
			 */
			VSLb(bo->vsl, SLT_Error,
			    "Illegal 'Vary' header from backend, "
			    "making this a pass.");
			bo->uncacheable = 1;
			AZ(vary);
		} else
			/* No vary */
			AZ(vary);
	}

	l2 = http_EstimateWS(bo->beresp,
	    bo->uncacheable ? HTTPH_A_PASS : HTTPH_A_INS);
	l += l2;

	if (bo->uncacheable)
		bo->fetch_objcore->flags |= OC_F_PASS;

	if (!vbf_allocobj(bo, l))
		return (-1);

	if (vary != NULL) {
		AN(ObjSetAttr(bo->wrk, bo->fetch_objcore, OA_VARY, varyl,
			VSB_data(vary)));
		VSB_destroy(&vary);
	}

	AZ(ObjSetU32(bo->wrk, bo->fetch_objcore, OA_VXID, VXID(bo->vsl->wid)));

	/* for HTTP_Encode() VSLH call */
	bo->beresp->logtag = SLT_ObjMethod;

	/* Filter into object */
	bp = ObjSetAttr(bo->wrk, bo->fetch_objcore, OA_HEADERS, l2, NULL);
	AN(bp);
	HTTP_Encode(bo->beresp, bp, l2,
	    bo->uncacheable ? HTTPH_A_PASS : HTTPH_A_INS);

	if (http_GetHdr(bo->beresp, H_Last_Modified, &b))
		AZ(ObjSetDouble(bo->wrk, bo->fetch_objcore, OA_LASTMODIFIED,
		    VTIM_parse(b)));
	else
		AZ(ObjSetDouble(bo->wrk, bo->fetch_objcore, OA_LASTMODIFIED,
		    floor(bo->fetch_objcore->t_origin)));

	return (0);
}
