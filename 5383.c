static int kfunc_btf_cmp_by_off(const void *a, const void *b)
{
	const struct bpf_kfunc_btf *d0 = a;
	const struct bpf_kfunc_btf *d1 = b;

	return d0->offset - d1->offset;
}