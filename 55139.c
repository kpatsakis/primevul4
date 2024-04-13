static inline void kvmppc_set_vsr_dword(struct kvm_vcpu *vcpu,
	u64 gpr)
{
	union kvmppc_one_reg val;
	int offset = kvmppc_get_vsr_dword_offset(vcpu->arch.mmio_vsx_offset);
	int index = vcpu->arch.io_gpr & KVM_MMIO_REG_MASK;

	if (offset == -1)
		return;

	if (vcpu->arch.mmio_vsx_tx_sx_enabled) {
		val.vval = VCPU_VSX_VR(vcpu, index);
		val.vsxval[offset] = gpr;
		VCPU_VSX_VR(vcpu, index) = val.vval;
	} else {
		VCPU_VSX_FPR(vcpu, index, offset) = gpr;
	}
}
