static int emulator_set_dr(struct x86_emulate_ctxt *ctxt, int dr,
			   unsigned long value)
{

	return __kvm_set_dr(emul_to_vcpu(ctxt), dr, value);
}
