static void lzw_emit_code(struct iwgifrcontext *rctx, struct lzwdeccontext *d,
		unsigned int first_code)
{
	unsigned int code;
	code = first_code;


	while(1) {
		iwgif_record_pixel(rctx, (unsigned int)d->ct[code].lastchar, (int)(d->ct[code].length-1));
		if(d->ct[code].length<=1) break;
		code = (unsigned int)d->ct[code].parent;
	}

	rctx->pixels_set += d->ct[first_code].length;
}
