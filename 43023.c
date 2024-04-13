static int emulator_check_pmc(struct x86_emulate_ctxt *ctxt,
			      u32 pmc)
{
	return kvm_pmu_check_pmc(emul_to_vcpu(ctxt), pmc);
}
