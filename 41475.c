static u32 tg3_read_indirect_reg32(struct tg3 *tp, u32 off)
{
	unsigned long flags;
	u32 val;

	spin_lock_irqsave(&tp->indirect_lock, flags);
	pci_write_config_dword(tp->pdev, TG3PCI_REG_BASE_ADDR, off);
	pci_read_config_dword(tp->pdev, TG3PCI_REG_DATA, &val);
	spin_unlock_irqrestore(&tp->indirect_lock, flags);
	return val;
}
