static int lzw_process_code(struct iwgifrcontext *rctx, struct lzwdeccontext *d,
		unsigned int code)
{
	if(code==d->eoi_code) {
		d->eoi_flag=1;
		return 1;
	}

	if(code==d->clear_code) {
		lzw_clear(d);
		return 1;
	}

	d->ncodes_since_clear++;

	if(d->ncodes_since_clear==1) {
		lzw_emit_code(rctx,d,code);
		d->oldcode = code;
		return 1;
	}

	if(code < d->ct_used) {
		lzw_emit_code(rctx,d,code);

		lzw_add_to_dict(d,d->oldcode,d->ct[code].firstchar);
	}
	else {
		if(d->oldcode>=d->ct_used) {
			iw_set_error(rctx->ctx,"GIF decoding error");
			return 0;
		}

		if(lzw_add_to_dict(d,d->oldcode,d->ct[d->oldcode].firstchar)) {
			lzw_emit_code(rctx,d,d->last_code_added);
		}
	}
	d->oldcode = code;

	return 1;
}
