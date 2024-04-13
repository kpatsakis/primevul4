static int kfunc_desc_cmp_by_imm(const void *a, const void *b)
{
	const struct bpf_kfunc_desc *d0 = a;
	const struct bpf_kfunc_desc *d1 = b;

	if (d0->imm > d1->imm)
		return 1;
	else if (d0->imm < d1->imm)
		return -1;
	return 0;
}