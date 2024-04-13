static int get_msr_hyperv(struct kvm_vcpu *vcpu, u32 msr, u64 *pdata)
{
	u64 data = 0;

	switch (msr) {
	case HV_X64_MSR_VP_INDEX: {
		int r;
		struct kvm_vcpu *v;
		kvm_for_each_vcpu(r, v, vcpu->kvm) {
			if (v == vcpu) {
				data = r;
				break;
			}
		}
		break;
	}
	case HV_X64_MSR_EOI:
		return kvm_hv_vapic_msr_read(vcpu, APIC_EOI, pdata);
	case HV_X64_MSR_ICR:
		return kvm_hv_vapic_msr_read(vcpu, APIC_ICR, pdata);
	case HV_X64_MSR_TPR:
		return kvm_hv_vapic_msr_read(vcpu, APIC_TASKPRI, pdata);
	case HV_X64_MSR_APIC_ASSIST_PAGE:
		data = vcpu->arch.hv_vapic;
		break;
	default:
		vcpu_unimpl(vcpu, "Hyper-V unhandled rdmsr: 0x%x\n", msr);
		return 1;
	}
	*pdata = data;
	return 0;
}
