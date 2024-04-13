vbf_stp_mkbereq(struct worker *wrk, struct busyobj *bo)
{
	const char *q;

	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
	CHECK_OBJ_NOTNULL(bo, BUSYOBJ_MAGIC);
	CHECK_OBJ_NOTNULL(bo->req, REQ_MAGIC);

	assert(bo->fetch_objcore->boc->state == BOS_INVALID);
	AZ(bo->storage);
	AZ(bo->storage_hint);

	HTTP_Setup(bo->bereq0, bo->ws, bo->vsl, SLT_BereqMethod);
	http_FilterReq(bo->bereq0, bo->req->http,
	    bo->do_pass ? HTTPH_R_PASS : HTTPH_R_FETCH);

	if (!bo->do_pass) {
		http_ForceField(bo->bereq0, HTTP_HDR_METHOD, "GET");
		http_ForceField(bo->bereq0, HTTP_HDR_PROTO, "HTTP/1.1");
		if (cache_param->http_gzip_support)
			http_ForceHeader(bo->bereq0, H_Accept_Encoding, "gzip");
	} else {
		AZ(bo->stale_oc);
		if (bo->bereq0->protover > 11)
			http_ForceField(bo->bereq0, HTTP_HDR_PROTO, "HTTP/1.1");
	}
	http_CopyHome(bo->bereq0);

	if (bo->stale_oc != NULL &&
	    ObjCheckFlag(bo->wrk, bo->stale_oc, OF_IMSCAND) &&
	    (bo->stale_oc->boc != NULL || ObjGetLen(wrk, bo->stale_oc) != 0)) {
		AZ(bo->stale_oc->flags & OC_F_PASS);
		q = HTTP_GetHdrPack(bo->wrk, bo->stale_oc, H_Last_Modified);
		if (q != NULL)
			http_PrintfHeader(bo->bereq0,
			    "If-Modified-Since: %s", q);
		q = HTTP_GetHdrPack(bo->wrk, bo->stale_oc, H_ETag);
		if (q != NULL)
			http_PrintfHeader(bo->bereq0,
			    "If-None-Match: %s", q);
	}

	HTTP_Setup(bo->bereq, bo->ws, bo->vsl, SLT_BereqMethod);
	bo->ws_bo = WS_Snapshot(bo->ws);
	HTTP_Copy(bo->bereq, bo->bereq0);

	if (bo->req->req_body_status == REQ_BODY_NONE) {
		bo->req = NULL;
		ObjSetState(bo->wrk, bo->fetch_objcore, BOS_REQ_DONE);
	}
	return (F_STP_STARTFETCH);
}
