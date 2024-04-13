static int handle_pcommit(struct kvm_vcpu *vcpu)
{
	/* we never catch pcommit instruct for L1 guest. */
	WARN_ON(1);
	return 1;
}
