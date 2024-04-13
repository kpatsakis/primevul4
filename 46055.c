int kvm_arch_vcpu_ioctl_run(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	int r;
	sigset_t sigsaved;

	if (vcpu->sigset_active)
		sigprocmask(SIG_SETMASK, &vcpu->sigset, &sigsaved);

	if (unlikely(vcpu->arch.mp_state == KVM_MP_STATE_UNINITIALIZED)) {
		kvm_vcpu_block(vcpu);
		clear_bit(KVM_REQ_UNHALT, &vcpu->requests);
		r = -EAGAIN;
		goto out;
	}

	/* re-sync apic's tpr */
	if (!irqchip_in_kernel(vcpu->kvm))
		kvm_set_cr8(vcpu, kvm_run->cr8);

	if (vcpu->arch.pio.count || vcpu->mmio_needed) {
		if (vcpu->mmio_needed) {
			memcpy(vcpu->mmio_data, kvm_run->mmio.data, 8);
			vcpu->mmio_read_completed = 1;
			vcpu->mmio_needed = 0;
		}
		vcpu->srcu_idx = srcu_read_lock(&vcpu->kvm->srcu);
		r = emulate_instruction(vcpu, 0, 0, EMULTYPE_NO_DECODE);
		srcu_read_unlock(&vcpu->kvm->srcu, vcpu->srcu_idx);
		if (r != EMULATE_DONE) {
			r = 0;
			goto out;
		}
	}
	if (kvm_run->exit_reason == KVM_EXIT_HYPERCALL)
		kvm_register_write(vcpu, VCPU_REGS_RAX,
				     kvm_run->hypercall.ret);

	r = __vcpu_run(vcpu);

out:
	post_kvm_run_save(vcpu);
	if (vcpu->sigset_active)
		sigprocmask(SIG_SETMASK, &sigsaved, NULL);

	return r;
}
