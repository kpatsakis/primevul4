static void process_smi_save_state_64(struct kvm_vcpu *vcpu, char *buf)
{
#ifdef CONFIG_X86_64
	struct desc_ptr dt;
	struct kvm_segment seg;
	unsigned long val;
	int i;

	for (i = 0; i < 16; i++)
		put_smstate(u64, buf, 0x7ff8 - i * 8, kvm_register_read(vcpu, i));

	put_smstate(u64, buf, 0x7f78, kvm_rip_read(vcpu));
	put_smstate(u32, buf, 0x7f70, kvm_get_rflags(vcpu));

	kvm_get_dr(vcpu, 6, &val);
	put_smstate(u64, buf, 0x7f68, val);
	kvm_get_dr(vcpu, 7, &val);
	put_smstate(u64, buf, 0x7f60, val);

	put_smstate(u64, buf, 0x7f58, kvm_read_cr0(vcpu));
	put_smstate(u64, buf, 0x7f50, kvm_read_cr3(vcpu));
	put_smstate(u64, buf, 0x7f48, kvm_read_cr4(vcpu));

	put_smstate(u32, buf, 0x7f00, vcpu->arch.smbase);

	/* revision id */
	put_smstate(u32, buf, 0x7efc, 0x00020064);

	put_smstate(u64, buf, 0x7ed0, vcpu->arch.efer);

	kvm_get_segment(vcpu, &seg, VCPU_SREG_TR);
	put_smstate(u16, buf, 0x7e90, seg.selector);
	put_smstate(u16, buf, 0x7e92, process_smi_get_segment_flags(&seg) >> 8);
	put_smstate(u32, buf, 0x7e94, seg.limit);
	put_smstate(u64, buf, 0x7e98, seg.base);

	kvm_x86_ops->get_idt(vcpu, &dt);
	put_smstate(u32, buf, 0x7e84, dt.size);
	put_smstate(u64, buf, 0x7e88, dt.address);

	kvm_get_segment(vcpu, &seg, VCPU_SREG_LDTR);
	put_smstate(u16, buf, 0x7e70, seg.selector);
	put_smstate(u16, buf, 0x7e72, process_smi_get_segment_flags(&seg) >> 8);
	put_smstate(u32, buf, 0x7e74, seg.limit);
	put_smstate(u64, buf, 0x7e78, seg.base);

	kvm_x86_ops->get_gdt(vcpu, &dt);
	put_smstate(u32, buf, 0x7e64, dt.size);
	put_smstate(u64, buf, 0x7e68, dt.address);

	for (i = 0; i < 6; i++)
		process_smi_save_seg_64(vcpu, buf, i);
#else
	WARN_ON_ONCE(1);
#endif
}
