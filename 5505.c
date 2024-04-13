void bpf_free_kfunc_btf_tab(struct bpf_kfunc_btf_tab *tab)
{
	if (!tab)
		return;

	while (tab->nr_descs--) {
		module_put(tab->descs[tab->nr_descs].module);
		btf_put(tab->descs[tab->nr_descs].btf);
	}
	kfree(tab);
}