static int destroy(RBinFile *arch) {
	r_bin_bflt_free ((struct r_bin_bflt_obj*)arch->o->bin_obj);
	return true;
}
