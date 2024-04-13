static void post_kvm_run_save(struct kvm_vcpu *vcpu)
{
	struct kvm_run *kvm_run = vcpu->run;

	kvm_run->if_flag = (kvm_get_rflags(vcpu) & X86_EFLAGS_IF) != 0;
	kvm_run->flags = is_smm(vcpu) ? KVM_RUN_X86_SMM : 0;
	kvm_run->cr8 = kvm_get_cr8(vcpu);
	kvm_run->apic_base = kvm_get_apic_base(vcpu);
	kvm_run->ready_for_interrupt_injection =
		pic_in_kernel(vcpu->kvm) ||
		kvm_vcpu_ready_for_interrupt_injection(vcpu);
}
