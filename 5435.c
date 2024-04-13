find_kfunc_desc(const struct bpf_prog *prog, u32 func_id, u16 offset)
{
	struct bpf_kfunc_desc desc = {
		.func_id = func_id,
		.offset = offset,
	};
	struct bpf_kfunc_desc_tab *tab;

	tab = prog->aux->kfunc_tab;
	return bsearch(&desc, tab->descs, tab->nr_descs,
		       sizeof(tab->descs[0]), kfunc_desc_cmp_by_id_off);
}