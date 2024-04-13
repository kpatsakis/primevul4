static int __kvmppc_handle_load(struct kvm_run *run, struct kvm_vcpu *vcpu,
				unsigned int rt, unsigned int bytes,
				int is_default_endian, int sign_extend)
{
	int idx, ret;
	bool host_swabbed;

	/* Pity C doesn't have a logical XOR operator */
	if (kvmppc_need_byteswap(vcpu)) {
		host_swabbed = is_default_endian;
	} else {
		host_swabbed = !is_default_endian;
	}

	if (bytes > sizeof(run->mmio.data)) {
		printk(KERN_ERR "%s: bad MMIO length: %d\n", __func__,
		       run->mmio.len);
	}

	run->mmio.phys_addr = vcpu->arch.paddr_accessed;
	run->mmio.len = bytes;
	run->mmio.is_write = 0;

	vcpu->arch.io_gpr = rt;
	vcpu->arch.mmio_host_swabbed = host_swabbed;
	vcpu->mmio_needed = 1;
	vcpu->mmio_is_write = 0;
	vcpu->arch.mmio_sign_extend = sign_extend;

	idx = srcu_read_lock(&vcpu->kvm->srcu);

	ret = kvm_io_bus_read(vcpu, KVM_MMIO_BUS, run->mmio.phys_addr,
			      bytes, &run->mmio.data);

	srcu_read_unlock(&vcpu->kvm->srcu, idx);

	if (!ret) {
		kvmppc_complete_mmio_load(vcpu, run);
		vcpu->mmio_needed = 0;
		return EMULATE_DONE;
	}

	return EMULATE_DO_MMIO;
}
