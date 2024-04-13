static void emulator_wbinvd(struct x86_emulate_ctxt *ctxt)
{
	kvm_emulate_wbinvd_noskip(emul_to_vcpu(ctxt));
}
