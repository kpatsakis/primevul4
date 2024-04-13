static void emulator_set_nmi_mask(struct x86_emulate_ctxt *ctxt, bool masked)
{
	kvm_x86_ops->set_nmi_mask(emul_to_vcpu(ctxt), masked);
}
