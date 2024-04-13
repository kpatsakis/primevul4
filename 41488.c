static void tg3_restore_pci_state(struct tg3 *tp)
{
	u32 val;

	/* Re-enable indirect register accesses. */
	pci_write_config_dword(tp->pdev, TG3PCI_MISC_HOST_CTRL,
			       tp->misc_host_ctrl);

	/* Set MAX PCI retry to zero. */
	val = (PCISTATE_ROM_ENABLE | PCISTATE_ROM_RETRY_ENABLE);
	if (tg3_chip_rev_id(tp) == CHIPREV_ID_5704_A0 &&
	    tg3_flag(tp, PCIX_MODE))
		val |= PCISTATE_RETRY_SAME_DMA;
	/* Allow reads and writes to the APE register and memory space. */
	if (tg3_flag(tp, ENABLE_APE))
		val |= PCISTATE_ALLOW_APE_CTLSPC_WR |
		       PCISTATE_ALLOW_APE_SHMEM_WR |
		       PCISTATE_ALLOW_APE_PSPACE_WR;
	pci_write_config_dword(tp->pdev, TG3PCI_PCISTATE, val);

	pci_write_config_word(tp->pdev, PCI_COMMAND, tp->pci_cmd);

	if (!tg3_flag(tp, PCI_EXPRESS)) {
		pci_write_config_byte(tp->pdev, PCI_CACHE_LINE_SIZE,
				      tp->pci_cacheline_sz);
		pci_write_config_byte(tp->pdev, PCI_LATENCY_TIMER,
				      tp->pci_lat_timer);
	}

	/* Make sure PCI-X relaxed ordering bit is clear. */
	if (tg3_flag(tp, PCIX_MODE)) {
		u16 pcix_cmd;

		pci_read_config_word(tp->pdev, tp->pcix_cap + PCI_X_CMD,
				     &pcix_cmd);
		pcix_cmd &= ~PCI_X_CMD_ERO;
		pci_write_config_word(tp->pdev, tp->pcix_cap + PCI_X_CMD,
				      pcix_cmd);
	}

	if (tg3_flag(tp, 5780_CLASS)) {

		/* Chip reset on 5780 will reset MSI enable bit,
		 * so need to restore it.
		 */
		if (tg3_flag(tp, USING_MSI)) {
			u16 ctrl;

			pci_read_config_word(tp->pdev,
					     tp->msi_cap + PCI_MSI_FLAGS,
					     &ctrl);
			pci_write_config_word(tp->pdev,
					      tp->msi_cap + PCI_MSI_FLAGS,
					      ctrl | PCI_MSI_FLAGS_ENABLE);
			val = tr32(MSGINT_MODE);
			tw32(MSGINT_MODE, val | MSGINT_MODE_ENABLE);
		}
	}
}
