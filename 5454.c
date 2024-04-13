bool bpf_prog_has_kfunc_call(const struct bpf_prog *prog)
{
	return !!prog->aux->kfunc_tab;
}