static int kfunc_desc_cmp_by_id_off(const void *a, const void *b)
{
	const struct bpf_kfunc_desc *d0 = a;
	const struct bpf_kfunc_desc *d1 = b;

	/* func_id is not greater than BTF_MAX_TYPE */
	return d0->func_id - d1->func_id ?: d0->offset - d1->offset;
}