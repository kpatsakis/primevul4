void kvm_arch_sched_in(struct kvm_vcpu *vcpu, int cpu)
{
	kvm_x86_ops->sched_in(vcpu, cpu);
}
