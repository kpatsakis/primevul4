static s32 atl2_reset_hw(struct atl2_hw *hw)
{
	u32 icr;
	u16 pci_cfg_cmd_word;
	int i;

	/* Workaround for PCI problem when BIOS sets MMRBC incorrectly. */
	atl2_read_pci_cfg(hw, PCI_REG_COMMAND, &pci_cfg_cmd_word);
	if ((pci_cfg_cmd_word &
		(CMD_IO_SPACE|CMD_MEMORY_SPACE|CMD_BUS_MASTER)) !=
		(CMD_IO_SPACE|CMD_MEMORY_SPACE|CMD_BUS_MASTER)) {
		pci_cfg_cmd_word |=
			(CMD_IO_SPACE|CMD_MEMORY_SPACE|CMD_BUS_MASTER);
		atl2_write_pci_cfg(hw, PCI_REG_COMMAND, &pci_cfg_cmd_word);
	}

	/* Clear Interrupt mask to stop board from generating
	 * interrupts & Clear any pending interrupt events
	 */
	/* FIXME */
	/* ATL2_WRITE_REG(hw, REG_IMR, 0); */
	/* ATL2_WRITE_REG(hw, REG_ISR, 0xffffffff); */

	/* Issue Soft Reset to the MAC.  This will reset the chip's
	 * transmit, receive, DMA.  It will not effect
	 * the current PCI configuration.  The global reset bit is self-
	 * clearing, and should clear within a microsecond.
	 */
	ATL2_WRITE_REG(hw, REG_MASTER_CTRL, MASTER_CTRL_SOFT_RST);
	wmb();
	msleep(1); /* delay about 1ms */

	/* Wait at least 10ms for All module to be Idle */
	for (i = 0; i < 10; i++) {
		icr = ATL2_READ_REG(hw, REG_IDLE_STATUS);
		if (!icr)
			break;
		msleep(1); /* delay 1 ms */
		cpu_relax();
	}

	if (icr)
		return icr;

	return 0;
}
