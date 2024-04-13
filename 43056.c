static bool kvm_hv_msr_partition_wide(u32 msr)
{
	bool r = false;
	switch (msr) {
	case HV_X64_MSR_GUEST_OS_ID:
	case HV_X64_MSR_HYPERCALL:
	case HV_X64_MSR_REFERENCE_TSC:
	case HV_X64_MSR_TIME_REF_COUNT:
		r = true;
		break;
	}

	return r;
}
