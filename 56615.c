static int nested_vmx_check_io_bitmap_controls(struct kvm_vcpu *vcpu,
					       struct vmcs12 *vmcs12)
{
	if (!nested_cpu_has(vmcs12, CPU_BASED_USE_IO_BITMAPS))
		return 0;

	if (!page_address_valid(vcpu, vmcs12->io_bitmap_a) ||
	    !page_address_valid(vcpu, vmcs12->io_bitmap_b))
		return -EINVAL;

	return 0;
}
