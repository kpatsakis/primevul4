static RList *entries(RBinFile *arch) {
	struct r_bin_bflt_obj *obj = (struct r_bin_bflt_obj*)arch->o->bin_obj;
	RList *ret;
	RBinAddr *ptr;

	if (!(ret = r_list_newf (free))) {
		return NULL;
	}
	ptr = r_bflt_get_entry (obj);
	if (!ptr) {
		return NULL;
	}
	r_list_append (ret, ptr);
	return ret;
}
