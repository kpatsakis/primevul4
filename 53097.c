static void atl2_setup_pcicmd(struct pci_dev *pdev)
{
	u16 cmd;

	pci_read_config_word(pdev, PCI_COMMAND, &cmd);

	if (cmd & PCI_COMMAND_INTX_DISABLE)
		cmd &= ~PCI_COMMAND_INTX_DISABLE;
	if (cmd & PCI_COMMAND_IO)
		cmd &= ~PCI_COMMAND_IO;
	if (0 == (cmd & PCI_COMMAND_MEMORY))
		cmd |= PCI_COMMAND_MEMORY;
	if (0 == (cmd & PCI_COMMAND_MASTER))
		cmd |= PCI_COMMAND_MASTER;
	pci_write_config_word(pdev, PCI_COMMAND, cmd);

	/*
	 * some motherboards BIOS(PXE/EFI) driver may set PME
	 * while they transfer control to OS (Windows/Linux)
	 * so we should clear this bit before NIC work normally
	 */
	pci_write_config_dword(pdev, REG_PM_CTRLSTAT, 0);
}
