static int emulator_get_dr(struct x86_emulate_ctxt *ctxt, int dr,
			   unsigned long *dest)
{
	return kvm_get_dr(emul_to_vcpu(ctxt), dr, dest);
}
