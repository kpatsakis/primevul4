static RBinInfo *info(RBinFile *arch) {
	RBinHash *h;
	RBinInfo *ret = R_NEW0 (RBinInfo);
	if (!ret) {
		return NULL;
	}
	ret->file = arch->file? strdup (arch->file): NULL;
	ret->type = strdup ("DEX CLASS");
	ret->has_va = false;
	ret->bclass = r_bin_dex_get_version (arch->o->bin_obj);
	ret->rclass = strdup ("class");
	ret->os = strdup ("linux");
	ret->subsystem = strdup ("any");
	ret->machine = strdup ("Dalvik VM");
	h = &ret->sum[0];
	h->type = "sha1";
	h->len = 20;
	h->addr = 12;
	h->from = 12;
	h->to = arch->buf->length-32;
	memcpy (h->buf, arch->buf->buf+12, 20);
	h = &ret->sum[1];
	h->type = "adler32";
	h->len = 4;
	h->addr = 0x8;
	h->from = 12;
	h->to = arch->buf->length-h->from;
	h = &ret->sum[2];
	h->type = 0;
	memcpy (h->buf, arch->buf->buf + 8, 4);
	{
		ut32 *fc = (ut32 *)(arch->buf->buf + 8);
		ut32  cc = __adler32 (arch->buf->buf + 12, arch->buf->length - 12);
		if (*fc != cc) {
			eprintf ("# adler32 checksum doesn't match. Type this to fix it:\n");
			eprintf ("wx `#sha1 $s-32 @32` @12 ; wx `#adler32 $s-12 @12` @8\n");
		}
	}
	ret->arch = strdup ("dalvik");
	ret->lang = "dalvik";
	ret->bits = 32;
	ret->big_endian = 0;
	ret->dbg_info = 0; //1 | 4 | 8; /* Stripped | LineNums | Syms */
	return ret;
}
