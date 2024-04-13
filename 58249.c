static void lzw_init(struct lzwdeccontext *d, unsigned int root_codesize)
{
	unsigned int i;

	iw_zeromem(d,sizeof(struct lzwdeccontext));

	d->root_codesize = root_codesize;
	d->num_root_codes = 1<<d->root_codesize;
	d->clear_code = d->num_root_codes;
	d->eoi_code = d->num_root_codes+1;
	for(i=0;i<d->num_root_codes;i++) {
		d->ct[i].parent = 0;
		d->ct[i].length = 1;
		d->ct[i].lastchar = (iw_byte)i;
		d->ct[i].firstchar = (iw_byte)i;
	}
}
