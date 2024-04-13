static void fix_rmode_seg(int seg, struct kvm_segment *save)
{
	const struct kvm_vmx_segment_field *sf = &kvm_vmx_segment_fields[seg];
	struct kvm_segment var = *save;

	var.dpl = 0x3;
	if (seg == VCPU_SREG_CS)
		var.type = 0x3;

	if (!emulate_invalid_guest_state) {
		var.selector = var.base >> 4;
		var.base = var.base & 0xffff0;
		var.limit = 0xffff;
		var.g = 0;
		var.db = 0;
		var.present = 1;
		var.s = 1;
		var.l = 0;
		var.unusable = 0;
		var.type = 0x3;
		var.avl = 0;
		if (save->base & 0xf)
			printk_once(KERN_WARNING "kvm: segment base is not "
					"paragraph aligned when entering "
					"protected mode (seg=%d)", seg);
	}

	vmcs_write16(sf->selector, var.selector);
	vmcs_writel(sf->base, var.base);
	vmcs_write32(sf->limit, var.limit);
	vmcs_write32(sf->ar_bytes, vmx_segment_access_rights(&var));
}
