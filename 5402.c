bpf_jit_find_kfunc_model(const struct bpf_prog *prog,
			 const struct bpf_insn *insn)
{
	const struct bpf_kfunc_desc desc = {
		.imm = insn->imm,
	};
	const struct bpf_kfunc_desc *res;
	struct bpf_kfunc_desc_tab *tab;

	tab = prog->aux->kfunc_tab;
	res = bsearch(&desc, tab->descs, tab->nr_descs,
		      sizeof(tab->descs[0]), kfunc_desc_cmp_by_imm);

	return res ? &res->func_model : NULL;
}