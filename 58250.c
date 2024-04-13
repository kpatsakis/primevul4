static int lzw_process_bytes(struct iwgifrcontext *rctx, struct lzwdeccontext *d,
	iw_byte *data, size_t data_size)
{
	size_t i;
	int b;
	int retval=0;

	for(i=0;i<data_size;i++) {
		for(b=0;b<8;b++) {
			if(d->eoi_flag) { // Stop if we've seen an EOI (end of image) code.
				retval=1;
				goto done;
			}

			if(data[i]&(1<<b))
				d->pending_code |= 1<<d->bits_in_pending_code;
			d->bits_in_pending_code++;

			if(d->bits_in_pending_code >= d->current_codesize) {
				if(!lzw_process_code(rctx,d,d->pending_code)) goto done;
				d->pending_code=0;
				d->bits_in_pending_code=0;
			}
		}
	}
	retval=1;

done:
	return retval;
}
