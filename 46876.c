static int nested_vmx_check_msr_bitmap_controls(struct kvm_vcpu *vcpu,
						struct vmcs12 *vmcs12)
{
	int maxphyaddr;
	u64 addr;

	if (!nested_cpu_has(vmcs12, CPU_BASED_USE_MSR_BITMAPS))
		return 0;

	if (vmcs12_read_any(vcpu, MSR_BITMAP, &addr)) {
		WARN_ON(1);
		return -EINVAL;
	}
	maxphyaddr = cpuid_maxphyaddr(vcpu);

	if (!PAGE_ALIGNED(vmcs12->msr_bitmap) ||
	   ((addr + PAGE_SIZE) >> maxphyaddr))
		return -EINVAL;

	return 0;
}
