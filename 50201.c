static int ioapic_mmio_write(struct kvm_vcpu *vcpu, struct kvm_io_device *this,
				 gpa_t addr, int len, const void *val)
{
	struct kvm_ioapic *ioapic = to_ioapic(this);
	u32 data;
	if (!ioapic_in_range(ioapic, addr))
		return -EOPNOTSUPP;

	ioapic_debug("ioapic_mmio_write addr=%p len=%d val=%p\n",
		     (void*)addr, len, val);
	ASSERT(!(addr & 0xf));	/* check alignment */

	switch (len) {
	case 8:
	case 4:
		data = *(u32 *) val;
		break;
	case 2:
		data = *(u16 *) val;
		break;
	case 1:
		data = *(u8  *) val;
		break;
	default:
		printk(KERN_WARNING "ioapic: Unsupported size %d\n", len);
		return 0;
	}

	addr &= 0xff;
	spin_lock(&ioapic->lock);
	switch (addr) {
	case IOAPIC_REG_SELECT:
		ioapic->ioregsel = data & 0xFF; /* 8-bit register */
		break;

	case IOAPIC_REG_WINDOW:
		ioapic_write_indirect(ioapic, data);
		break;

	default:
		break;
	}
	spin_unlock(&ioapic->lock);
	return 0;
}
