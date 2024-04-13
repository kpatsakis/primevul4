static inline void kvmppc_set_vsr_word(struct kvm_vcpu *vcpu,
	u32 gpr32)
{
	union kvmppc_one_reg val;
	int offset = kvmppc_get_vsr_word_offset(vcpu->arch.mmio_vsx_offset);
	int index = vcpu->arch.io_gpr & KVM_MMIO_REG_MASK;
	int dword_offset, word_offset;

	if (offset == -1)
		return;

	if (vcpu->arch.mmio_vsx_tx_sx_enabled) {
		val.vval = VCPU_VSX_VR(vcpu, index);
		val.vsx32val[offset] = gpr32;
		VCPU_VSX_VR(vcpu, index) = val.vval;
	} else {
		dword_offset = offset / 2;
		word_offset = offset % 2;
		val.vsxval[0] = VCPU_VSX_FPR(vcpu, index, dword_offset);
		val.vsx32val[word_offset] = gpr32;
		VCPU_VSX_FPR(vcpu, index, dword_offset) = val.vsxval[0];
	}
}
