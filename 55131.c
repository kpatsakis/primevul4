int kvmppc_handle_store(struct kvm_run *run, struct kvm_vcpu *vcpu,
			u64 val, unsigned int bytes, int is_default_endian)
{
	void *data = run->mmio.data;
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
	run->mmio.is_write = 1;
	vcpu->mmio_needed = 1;
	vcpu->mmio_is_write = 1;

	if ((vcpu->arch.mmio_sp64_extend) && (bytes == 4))
		val = dp_to_sp(val);

	/* Store the value at the lowest bytes in 'data'. */
	if (!host_swabbed) {
		switch (bytes) {
		case 8: *(u64 *)data = val; break;
		case 4: *(u32 *)data = val; break;
		case 2: *(u16 *)data = val; break;
		case 1: *(u8  *)data = val; break;
		}
	} else {
		switch (bytes) {
		case 8: *(u64 *)data = swab64(val); break;
		case 4: *(u32 *)data = swab32(val); break;
		case 2: *(u16 *)data = swab16(val); break;
		case 1: *(u8  *)data = val; break;
		}
	}

	idx = srcu_read_lock(&vcpu->kvm->srcu);

	ret = kvm_io_bus_write(vcpu, KVM_MMIO_BUS, run->mmio.phys_addr,
			       bytes, &run->mmio.data);

	srcu_read_unlock(&vcpu->kvm->srcu, idx);

	if (!ret) {
		vcpu->mmio_needed = 0;
		return EMULATE_DONE;
	}

	return EMULATE_DO_MMIO;
}
