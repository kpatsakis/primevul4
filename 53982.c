void kvm_vcpu_reset(struct kvm_vcpu *vcpu, bool init_event)
{
	vcpu->arch.hflags = 0;

	atomic_set(&vcpu->arch.nmi_queued, 0);
	vcpu->arch.nmi_pending = 0;
	vcpu->arch.nmi_injected = false;
	kvm_clear_interrupt_queue(vcpu);
	kvm_clear_exception_queue(vcpu);

	memset(vcpu->arch.db, 0, sizeof(vcpu->arch.db));
	kvm_update_dr0123(vcpu);
	vcpu->arch.dr6 = DR6_INIT;
	kvm_update_dr6(vcpu);
	vcpu->arch.dr7 = DR7_FIXED_1;
	kvm_update_dr7(vcpu);

	vcpu->arch.cr2 = 0;

	kvm_make_request(KVM_REQ_EVENT, vcpu);
	vcpu->arch.apf.msr_val = 0;
	vcpu->arch.st.msr_val = 0;

	kvmclock_reset(vcpu);

	kvm_clear_async_pf_completion_queue(vcpu);
	kvm_async_pf_hash_reset(vcpu);
	vcpu->arch.apf.halted = false;

	if (!init_event) {
		kvm_pmu_reset(vcpu);
		vcpu->arch.smbase = 0x30000;
	}

	memset(vcpu->arch.regs, 0, sizeof(vcpu->arch.regs));
	vcpu->arch.regs_avail = ~0;
	vcpu->arch.regs_dirty = ~0;

	kvm_x86_ops->vcpu_reset(vcpu, init_event);
}
