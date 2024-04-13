static int lzw_add_to_dict(struct lzwdeccontext *d, unsigned int oldcode, iw_byte val)
{
	static const unsigned int last_code_of_size[] = {
		0,0,0,7,15,31,63,127,255,511,1023,2047,4095
	};
	unsigned int newpos;

	if(d->ct_used>=4096) {
		d->last_code_added = 0;
		return 0;
	}

	newpos = d->ct_used;
	d->ct_used++;

	d->ct[newpos].parent = (iw_uint16)oldcode;
	d->ct[newpos].length = d->ct[oldcode].length + 1;
	d->ct[newpos].firstchar = d->ct[oldcode].firstchar;
	d->ct[newpos].lastchar = val;

	if(newpos == last_code_of_size[d->current_codesize]) {
		if(d->current_codesize<12) {
			d->current_codesize++;
		}
	}

	d->last_code_added = newpos;
	return 1;
}
