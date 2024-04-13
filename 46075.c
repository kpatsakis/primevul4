int kvm_inject_realmode_interrupt(struct kvm_vcpu *vcpu, int irq)
{
	struct decode_cache *c = &vcpu->arch.emulate_ctxt.decode;
	int ret;

	init_emulate_ctxt(vcpu);

	vcpu->arch.emulate_ctxt.decode.op_bytes = 2;
	vcpu->arch.emulate_ctxt.decode.ad_bytes = 2;
	vcpu->arch.emulate_ctxt.decode.eip = vcpu->arch.emulate_ctxt.eip;
	ret = emulate_int_real(&vcpu->arch.emulate_ctxt, &emulate_ops, irq);

	if (ret != X86EMUL_CONTINUE)
		return EMULATE_FAIL;

	vcpu->arch.emulate_ctxt.eip = c->eip;
	memcpy(vcpu->arch.regs, c->regs, sizeof c->regs);
	kvm_rip_write(vcpu, vcpu->arch.emulate_ctxt.eip);
	kvm_x86_ops->set_rflags(vcpu, vcpu->arch.emulate_ctxt.eflags);

	if (irq == NMI_VECTOR)
		vcpu->arch.nmi_pending = false;
	else
		vcpu->arch.interrupt.pending = false;

	return EMULATE_DONE;
}
