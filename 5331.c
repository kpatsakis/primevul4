static bool can_skip_alu_sanitation(const struct bpf_verifier_env *env,
				    const struct bpf_insn *insn)
{
	return env->bypass_spec_v1 || BPF_SRC(insn->code) == BPF_K;
}