static int nested_vmx_store_msr(struct kvm_vcpu *vcpu, u64 gpa, u32 count)
{
	u32 i;
	struct vmx_msr_entry e;

	for (i = 0; i < count; i++) {
		struct msr_data msr_info;
		if (kvm_vcpu_read_guest(vcpu,
					gpa + i * sizeof(e),
					&e, 2 * sizeof(u32))) {
			pr_warn_ratelimited(
				"%s cannot read MSR entry (%u, 0x%08llx)\n",
				__func__, i, gpa + i * sizeof(e));
			return -EINVAL;
		}
		if (nested_vmx_store_msr_check(vcpu, &e)) {
			pr_warn_ratelimited(
				"%s check failed (%u, 0x%x, 0x%x)\n",
				__func__, i, e.index, e.reserved);
			return -EINVAL;
		}
		msr_info.host_initiated = false;
		msr_info.index = e.index;
		if (kvm_get_msr(vcpu, &msr_info)) {
			pr_warn_ratelimited(
				"%s cannot read MSR (%u, 0x%x)\n",
				__func__, i, e.index);
			return -EINVAL;
		}
		if (kvm_vcpu_write_guest(vcpu,
					 gpa + i * sizeof(e) +
					     offsetof(struct vmx_msr_entry, value),
					 &msr_info.data, sizeof(msr_info.data))) {
			pr_warn_ratelimited(
				"%s cannot write MSR (%u, 0x%x, 0x%llx)\n",
				__func__, i, e.index, msr_info.data);
			return -EINVAL;
		}
	}
	return 0;
}
