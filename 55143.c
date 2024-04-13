static void kvmppc_swab_shared(struct kvm_vcpu *vcpu)
{
	struct kvm_vcpu_arch_shared *shared = vcpu->arch.shared;
	int i;

	shared->sprg0 = swab64(shared->sprg0);
	shared->sprg1 = swab64(shared->sprg1);
	shared->sprg2 = swab64(shared->sprg2);
	shared->sprg3 = swab64(shared->sprg3);
	shared->srr0 = swab64(shared->srr0);
	shared->srr1 = swab64(shared->srr1);
	shared->dar = swab64(shared->dar);
	shared->msr = swab64(shared->msr);
	shared->dsisr = swab32(shared->dsisr);
	shared->int_pending = swab32(shared->int_pending);
	for (i = 0; i < ARRAY_SIZE(shared->sr); i++)
		shared->sr[i] = swab32(shared->sr[i]);
}
