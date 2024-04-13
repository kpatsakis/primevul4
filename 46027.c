int emulate_instruction(struct kvm_vcpu *vcpu,
			unsigned long cr2,
			u16 error_code,
			int emulation_type)
{
	int r;
	struct decode_cache *c = &vcpu->arch.emulate_ctxt.decode;

	kvm_clear_exception_queue(vcpu);
	vcpu->arch.mmio_fault_cr2 = cr2;
	/*
	 * TODO: fix emulate.c to use guest_read/write_register
	 * instead of direct ->regs accesses, can save hundred cycles
	 * on Intel for instructions that don't read/change RSP, for
	 * for example.
	 */
	cache_all_regs(vcpu);

	if (!(emulation_type & EMULTYPE_NO_DECODE)) {
		init_emulate_ctxt(vcpu);
		vcpu->arch.emulate_ctxt.interruptibility = 0;
		vcpu->arch.emulate_ctxt.have_exception = false;
		vcpu->arch.emulate_ctxt.perm_ok = false;

		r = x86_decode_insn(&vcpu->arch.emulate_ctxt);
		if (r == X86EMUL_PROPAGATE_FAULT)
			goto done;

		trace_kvm_emulate_insn_start(vcpu);

		/* Only allow emulation of specific instructions on #UD
		 * (namely VMMCALL, sysenter, sysexit, syscall)*/
		if (emulation_type & EMULTYPE_TRAP_UD) {
			if (!c->twobyte)
				return EMULATE_FAIL;
			switch (c->b) {
			case 0x01: /* VMMCALL */
				if (c->modrm_mod != 3 || c->modrm_rm != 1)
					return EMULATE_FAIL;
				break;
			case 0x34: /* sysenter */
			case 0x35: /* sysexit */
				if (c->modrm_mod != 0 || c->modrm_rm != 0)
					return EMULATE_FAIL;
				break;
			case 0x05: /* syscall */
				if (c->modrm_mod != 0 || c->modrm_rm != 0)
					return EMULATE_FAIL;
				break;
			default:
				return EMULATE_FAIL;
			}

			if (!(c->modrm_reg == 0 || c->modrm_reg == 3))
				return EMULATE_FAIL;
		}

		++vcpu->stat.insn_emulation;
		if (r)  {
			if (reexecute_instruction(vcpu, cr2))
				return EMULATE_DONE;
			if (emulation_type & EMULTYPE_SKIP)
				return EMULATE_FAIL;
			return handle_emulation_failure(vcpu);
		}
	}

	if (emulation_type & EMULTYPE_SKIP) {
		kvm_rip_write(vcpu, vcpu->arch.emulate_ctxt.decode.eip);
		return EMULATE_DONE;
	}

	/* this is needed for vmware backdor interface to work since it
	   changes registers values  during IO operation */
	memcpy(c->regs, vcpu->arch.regs, sizeof c->regs);

restart:
	r = x86_emulate_insn(&vcpu->arch.emulate_ctxt);

	if (r == EMULATION_FAILED) {
		if (reexecute_instruction(vcpu, cr2))
			return EMULATE_DONE;

		return handle_emulation_failure(vcpu);
	}

done:
	if (vcpu->arch.emulate_ctxt.have_exception) {
		inject_emulated_exception(vcpu);
		r = EMULATE_DONE;
	} else if (vcpu->arch.pio.count) {
		if (!vcpu->arch.pio.in)
			vcpu->arch.pio.count = 0;
		r = EMULATE_DO_MMIO;
	} else if (vcpu->mmio_needed) {
		if (vcpu->mmio_is_write)
			vcpu->mmio_needed = 0;
		r = EMULATE_DO_MMIO;
	} else if (r == EMULATION_RESTART)
		goto restart;
	else
		r = EMULATE_DONE;

	toggle_interruptibility(vcpu, vcpu->arch.emulate_ctxt.interruptibility);
	kvm_x86_ops->set_rflags(vcpu, vcpu->arch.emulate_ctxt.eflags);
	kvm_make_request(KVM_REQ_EVENT, vcpu);
	memcpy(vcpu->arch.regs, c->regs, sizeof c->regs);
	kvm_rip_write(vcpu, vcpu->arch.emulate_ctxt.eip);

	return r;
}
