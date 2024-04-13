static void sort_kfunc_descs_by_imm(struct bpf_prog *prog)
{
	struct bpf_kfunc_desc_tab *tab;

	tab = prog->aux->kfunc_tab;
	if (!tab)
		return;

	sort(tab->descs, tab->nr_descs, sizeof(tab->descs[0]),
	     kfunc_desc_cmp_by_imm, NULL);
}