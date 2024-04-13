static enum bpf_prog_type resolve_prog_type(struct bpf_prog *prog)
{
	return prog->aux->dst_prog ? prog->aux->dst_prog->type : prog->type;
}