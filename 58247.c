static void lzw_clear(struct lzwdeccontext *d)
{
	d->ct_used = d->num_root_codes+2;
	d->current_codesize = d->root_codesize+1;
	d->ncodes_since_clear=0;
	d->oldcode=0;
}
