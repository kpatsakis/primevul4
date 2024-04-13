static inline void kvmppc_set_vsr_dword_dump(struct kvm_vcpu *vcpu,
	u64 gpr)
{
	union kvmppc_one_reg val;
	int index = vcpu->arch.io_gpr & KVM_MMIO_REG_MASK;

	if (vcpu->arch.mmio_vsx_tx_sx_enabled) {
		val.vval = VCPU_VSX_VR(vcpu, index);
		val.vsxval[0] = gpr;
		val.vsxval[1] = gpr;
		VCPU_VSX_VR(vcpu, index) = val.vval;
	} else {
		VCPU_VSX_FPR(vcpu, index, 0) = gpr;
		VCPU_VSX_FPR(vcpu, index, 1) = gpr;
	}
}
