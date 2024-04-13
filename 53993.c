static void process_smi(struct kvm_vcpu *vcpu)
{
	struct kvm_segment cs, ds;
	struct desc_ptr dt;
	char buf[512];
	u32 cr0;

	if (is_smm(vcpu)) {
		vcpu->arch.smi_pending = true;
		return;
	}

	trace_kvm_enter_smm(vcpu->vcpu_id, vcpu->arch.smbase, true);
	vcpu->arch.hflags |= HF_SMM_MASK;
	memset(buf, 0, 512);
	if (guest_cpuid_has_longmode(vcpu))
		process_smi_save_state_64(vcpu, buf);
	else
		process_smi_save_state_32(vcpu, buf);

	kvm_vcpu_write_guest(vcpu, vcpu->arch.smbase + 0xfe00, buf, sizeof(buf));

	if (kvm_x86_ops->get_nmi_mask(vcpu))
		vcpu->arch.hflags |= HF_SMM_INSIDE_NMI_MASK;
	else
		kvm_x86_ops->set_nmi_mask(vcpu, true);

	kvm_set_rflags(vcpu, X86_EFLAGS_FIXED);
	kvm_rip_write(vcpu, 0x8000);

	cr0 = vcpu->arch.cr0 & ~(X86_CR0_PE | X86_CR0_EM | X86_CR0_TS | X86_CR0_PG);
	kvm_x86_ops->set_cr0(vcpu, cr0);
	vcpu->arch.cr0 = cr0;

	kvm_x86_ops->set_cr4(vcpu, 0);

	/* Undocumented: IDT limit is set to zero on entry to SMM.  */
	dt.address = dt.size = 0;
	kvm_x86_ops->set_idt(vcpu, &dt);

	__kvm_set_dr(vcpu, 7, DR7_FIXED_1);

	cs.selector = (vcpu->arch.smbase >> 4) & 0xffff;
	cs.base = vcpu->arch.smbase;

	ds.selector = 0;
	ds.base = 0;

	cs.limit    = ds.limit = 0xffffffff;
	cs.type     = ds.type = 0x3;
	cs.dpl      = ds.dpl = 0;
	cs.db       = ds.db = 0;
	cs.s        = ds.s = 1;
	cs.l        = ds.l = 0;
	cs.g        = ds.g = 1;
	cs.avl      = ds.avl = 0;
	cs.present  = ds.present = 1;
	cs.unusable = ds.unusable = 0;
	cs.padding  = ds.padding = 0;

	kvm_set_segment(vcpu, &cs, VCPU_SREG_CS);
	kvm_set_segment(vcpu, &ds, VCPU_SREG_DS);
	kvm_set_segment(vcpu, &ds, VCPU_SREG_ES);
	kvm_set_segment(vcpu, &ds, VCPU_SREG_FS);
	kvm_set_segment(vcpu, &ds, VCPU_SREG_GS);
	kvm_set_segment(vcpu, &ds, VCPU_SREG_SS);

	if (guest_cpuid_has_longmode(vcpu))
		kvm_x86_ops->set_efer(vcpu, 0);

	kvm_update_cpuid(vcpu);
	kvm_mmu_reset_context(vcpu);
}
