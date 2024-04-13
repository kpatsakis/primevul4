static inline int kvmppc_get_vsr_data(struct kvm_vcpu *vcpu, int rs, u64 *val)
{
	u32 dword_offset, word_offset;
	union kvmppc_one_reg reg;
	int vsx_offset = 0;
	int copy_type = vcpu->arch.mmio_vsx_copy_type;
	int result = 0;

	switch (copy_type) {
	case KVMPPC_VSX_COPY_DWORD:
		vsx_offset =
			kvmppc_get_vsr_dword_offset(vcpu->arch.mmio_vsx_offset);

		if (vsx_offset == -1) {
			result = -1;
			break;
		}

		if (!vcpu->arch.mmio_vsx_tx_sx_enabled) {
			*val = VCPU_VSX_FPR(vcpu, rs, vsx_offset);
		} else {
			reg.vval = VCPU_VSX_VR(vcpu, rs);
			*val = reg.vsxval[vsx_offset];
		}
		break;

	case KVMPPC_VSX_COPY_WORD:
		vsx_offset =
			kvmppc_get_vsr_word_offset(vcpu->arch.mmio_vsx_offset);

		if (vsx_offset == -1) {
			result = -1;
			break;
		}

		if (!vcpu->arch.mmio_vsx_tx_sx_enabled) {
			dword_offset = vsx_offset / 2;
			word_offset = vsx_offset % 2;
			reg.vsxval[0] = VCPU_VSX_FPR(vcpu, rs, dword_offset);
			*val = reg.vsx32val[word_offset];
		} else {
			reg.vval = VCPU_VSX_VR(vcpu, rs);
			*val = reg.vsx32val[vsx_offset];
		}
		break;

	default:
		result = -1;
		break;
	}

	return result;
}
