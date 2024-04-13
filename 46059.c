int kvm_arch_vcpu_reset(struct kvm_vcpu *vcpu)
{
	vcpu->arch.nmi_pending = false;
	vcpu->arch.nmi_injected = false;

	vcpu->arch.switch_db_regs = 0;
	memset(vcpu->arch.db, 0, sizeof(vcpu->arch.db));
	vcpu->arch.dr6 = DR6_FIXED_1;
	vcpu->arch.dr7 = DR7_FIXED_1;

	kvm_make_request(KVM_REQ_EVENT, vcpu);
	vcpu->arch.apf.msr_val = 0;

	kvm_clear_async_pf_completion_queue(vcpu);
	kvm_async_pf_hash_reset(vcpu);
	vcpu->arch.apf.halted = false;

	return kvm_x86_ops->vcpu_reset(vcpu);
}
