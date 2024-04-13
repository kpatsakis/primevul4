int kvm_mmu_page_fault(struct kvm_vcpu *vcpu, gva_t cr2, u32 error_code,
		       void *insn, int insn_len)
{
	int r, emulation_type = EMULTYPE_RETRY;
	enum emulation_result er;

	r = vcpu->arch.mmu.page_fault(vcpu, cr2, error_code, false);
	if (r < 0)
		goto out;

	if (!r) {
		r = 1;
		goto out;
	}

	if (is_mmio_page_fault(vcpu, cr2))
		emulation_type = 0;

	er = x86_emulate_instruction(vcpu, cr2, emulation_type, insn, insn_len);

	switch (er) {
	case EMULATE_DONE:
		return 1;
	case EMULATE_USER_EXIT:
		++vcpu->stat.mmio_exits;
		/* fall through */
	case EMULATE_FAIL:
		return 0;
	default:
		BUG();
	}
out:
	return r;
}
