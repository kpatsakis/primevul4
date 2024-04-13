static int get_msr_hyperv_pw(struct kvm_vcpu *vcpu, u32 msr, u64 *pdata)
{
	u64 data = 0;
	struct kvm *kvm = vcpu->kvm;

	switch (msr) {
	case HV_X64_MSR_GUEST_OS_ID:
		data = kvm->arch.hv_guest_os_id;
		break;
	case HV_X64_MSR_HYPERCALL:
		data = kvm->arch.hv_hypercall;
		break;
	case HV_X64_MSR_TIME_REF_COUNT: {
		data =
		     div_u64(get_kernel_ns() + kvm->arch.kvmclock_offset, 100);
		break;
	}
	case HV_X64_MSR_REFERENCE_TSC:
		data = kvm->arch.hv_tsc_page;
		break;
	default:
		vcpu_unimpl(vcpu, "Hyper-V unhandled rdmsr: 0x%x\n", msr);
		return 1;
	}

	*pdata = data;
	return 0;
}
