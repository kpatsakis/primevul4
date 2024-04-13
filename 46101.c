static void update_cpuid(struct kvm_vcpu *vcpu)
{
	struct kvm_cpuid_entry2 *best;

	best = kvm_find_cpuid_entry(vcpu, 1, 0);
	if (!best)
		return;

	/* Update OSXSAVE bit */
	if (cpu_has_xsave && best->function == 0x1) {
		best->ecx &= ~(bit(X86_FEATURE_OSXSAVE));
		if (kvm_read_cr4_bits(vcpu, X86_CR4_OSXSAVE))
			best->ecx |= bit(X86_FEATURE_OSXSAVE);
	}
}
