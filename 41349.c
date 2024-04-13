static int tg3_get_invariants(struct tg3 *tp, const struct pci_device_id *ent)
{
	u32 misc_ctrl_reg;
	u32 pci_state_reg, grc_misc_cfg;
	u32 val;
	u16 pci_cmd;
	int err;

	/* Force memory write invalidate off.  If we leave it on,
	 * then on 5700_BX chips we have to enable a workaround.
	 * The workaround is to set the TG3PCI_DMA_RW_CTRL boundary
	 * to match the cacheline size.  The Broadcom driver have this
	 * workaround but turns MWI off all the times so never uses
	 * it.  This seems to suggest that the workaround is insufficient.
	 */
	pci_read_config_word(tp->pdev, PCI_COMMAND, &pci_cmd);
	pci_cmd &= ~PCI_COMMAND_INVALIDATE;
	pci_write_config_word(tp->pdev, PCI_COMMAND, pci_cmd);

	/* Important! -- Make sure register accesses are byteswapped
	 * correctly.  Also, for those chips that require it, make
	 * sure that indirect register accesses are enabled before
	 * the first operation.
	 */
	pci_read_config_dword(tp->pdev, TG3PCI_MISC_HOST_CTRL,
			      &misc_ctrl_reg);
	tp->misc_host_ctrl |= (misc_ctrl_reg &
			       MISC_HOST_CTRL_CHIPREV);
	pci_write_config_dword(tp->pdev, TG3PCI_MISC_HOST_CTRL,
			       tp->misc_host_ctrl);

	tg3_detect_asic_rev(tp, misc_ctrl_reg);

	/* If we have 5702/03 A1 or A2 on certain ICH chipsets,
	 * we need to disable memory and use config. cycles
	 * only to access all registers. The 5702/03 chips
	 * can mistakenly decode the special cycles from the
	 * ICH chipsets as memory write cycles, causing corruption
	 * of register and memory space. Only certain ICH bridges
	 * will drive special cycles with non-zero data during the
	 * address phase which can fall within the 5703's address
	 * range. This is not an ICH bug as the PCI spec allows
	 * non-zero address during special cycles. However, only
	 * these ICH bridges are known to drive non-zero addresses
	 * during special cycles.
	 *
	 * Since special cycles do not cross PCI bridges, we only
	 * enable this workaround if the 5703 is on the secondary
	 * bus of these ICH bridges.
	 */
	if ((tg3_chip_rev_id(tp) == CHIPREV_ID_5703_A1) ||
	    (tg3_chip_rev_id(tp) == CHIPREV_ID_5703_A2)) {
		static struct tg3_dev_id {
			u32	vendor;
			u32	device;
			u32	rev;
		} ich_chipsets[] = {
			{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82801AA_8,
			  PCI_ANY_ID },
			{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82801AB_8,
			  PCI_ANY_ID },
			{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82801BA_11,
			  0xa },
			{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82801BA_6,
			  PCI_ANY_ID },
			{ },
		};
		struct tg3_dev_id *pci_id = &ich_chipsets[0];
		struct pci_dev *bridge = NULL;

		while (pci_id->vendor != 0) {
			bridge = pci_get_device(pci_id->vendor, pci_id->device,
						bridge);
			if (!bridge) {
				pci_id++;
				continue;
			}
			if (pci_id->rev != PCI_ANY_ID) {
				if (bridge->revision > pci_id->rev)
					continue;
			}
			if (bridge->subordinate &&
			    (bridge->subordinate->number ==
			     tp->pdev->bus->number)) {
				tg3_flag_set(tp, ICH_WORKAROUND);
				pci_dev_put(bridge);
				break;
			}
		}
	}

	if (tg3_asic_rev(tp) == ASIC_REV_5701) {
		static struct tg3_dev_id {
			u32	vendor;
			u32	device;
		} bridge_chipsets[] = {
			{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_PXH_0 },
			{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_PXH_1 },
			{ },
		};
		struct tg3_dev_id *pci_id = &bridge_chipsets[0];
		struct pci_dev *bridge = NULL;

		while (pci_id->vendor != 0) {
			bridge = pci_get_device(pci_id->vendor,
						pci_id->device,
						bridge);
			if (!bridge) {
				pci_id++;
				continue;
			}
			if (bridge->subordinate &&
			    (bridge->subordinate->number <=
			     tp->pdev->bus->number) &&
			    (bridge->subordinate->busn_res.end >=
			     tp->pdev->bus->number)) {
				tg3_flag_set(tp, 5701_DMA_BUG);
				pci_dev_put(bridge);
				break;
			}
		}
	}

	/* The EPB bridge inside 5714, 5715, and 5780 cannot support
	 * DMA addresses > 40-bit. This bridge may have other additional
	 * 57xx devices behind it in some 4-port NIC designs for example.
	 * Any tg3 device found behind the bridge will also need the 40-bit
	 * DMA workaround.
	 */
	if (tg3_flag(tp, 5780_CLASS)) {
		tg3_flag_set(tp, 40BIT_DMA_BUG);
		tp->msi_cap = pci_find_capability(tp->pdev, PCI_CAP_ID_MSI);
	} else {
		struct pci_dev *bridge = NULL;

		do {
			bridge = pci_get_device(PCI_VENDOR_ID_SERVERWORKS,
						PCI_DEVICE_ID_SERVERWORKS_EPB,
						bridge);
			if (bridge && bridge->subordinate &&
			    (bridge->subordinate->number <=
			     tp->pdev->bus->number) &&
			    (bridge->subordinate->busn_res.end >=
			     tp->pdev->bus->number)) {
				tg3_flag_set(tp, 40BIT_DMA_BUG);
				pci_dev_put(bridge);
				break;
			}
		} while (bridge);
	}

	if (tg3_asic_rev(tp) == ASIC_REV_5704 ||
	    tg3_asic_rev(tp) == ASIC_REV_5714)
		tp->pdev_peer = tg3_find_peer(tp);

	/* Determine TSO capabilities */
	if (tg3_chip_rev_id(tp) == CHIPREV_ID_5719_A0)
		; /* Do nothing. HW bug. */
	else if (tg3_flag(tp, 57765_PLUS))
		tg3_flag_set(tp, HW_TSO_3);
	else if (tg3_flag(tp, 5755_PLUS) ||
		 tg3_asic_rev(tp) == ASIC_REV_5906)
		tg3_flag_set(tp, HW_TSO_2);
	else if (tg3_flag(tp, 5750_PLUS)) {
		tg3_flag_set(tp, HW_TSO_1);
		tg3_flag_set(tp, TSO_BUG);
		if (tg3_asic_rev(tp) == ASIC_REV_5750 &&
		    tg3_chip_rev_id(tp) >= CHIPREV_ID_5750_C2)
			tg3_flag_clear(tp, TSO_BUG);
	} else if (tg3_asic_rev(tp) != ASIC_REV_5700 &&
		   tg3_asic_rev(tp) != ASIC_REV_5701 &&
		   tg3_chip_rev_id(tp) != CHIPREV_ID_5705_A0) {
			tg3_flag_set(tp, TSO_BUG);
		if (tg3_asic_rev(tp) == ASIC_REV_5705)
			tp->fw_needed = FIRMWARE_TG3TSO5;
		else
			tp->fw_needed = FIRMWARE_TG3TSO;
	}

	/* Selectively allow TSO based on operating conditions */
	if (tg3_flag(tp, HW_TSO_1) ||
	    tg3_flag(tp, HW_TSO_2) ||
	    tg3_flag(tp, HW_TSO_3) ||
	    tp->fw_needed) {
		/* For firmware TSO, assume ASF is disabled.
		 * We'll disable TSO later if we discover ASF
		 * is enabled in tg3_get_eeprom_hw_cfg().
		 */
		tg3_flag_set(tp, TSO_CAPABLE);
	} else {
		tg3_flag_clear(tp, TSO_CAPABLE);
		tg3_flag_clear(tp, TSO_BUG);
		tp->fw_needed = NULL;
	}

	if (tg3_chip_rev_id(tp) == CHIPREV_ID_5701_A0)
		tp->fw_needed = FIRMWARE_TG3;

	tp->irq_max = 1;

	if (tg3_flag(tp, 5750_PLUS)) {
		tg3_flag_set(tp, SUPPORT_MSI);
		if (tg3_chip_rev(tp) == CHIPREV_5750_AX ||
		    tg3_chip_rev(tp) == CHIPREV_5750_BX ||
		    (tg3_asic_rev(tp) == ASIC_REV_5714 &&
		     tg3_chip_rev_id(tp) <= CHIPREV_ID_5714_A2 &&
		     tp->pdev_peer == tp->pdev))
			tg3_flag_clear(tp, SUPPORT_MSI);

		if (tg3_flag(tp, 5755_PLUS) ||
		    tg3_asic_rev(tp) == ASIC_REV_5906) {
			tg3_flag_set(tp, 1SHOT_MSI);
		}

		if (tg3_flag(tp, 57765_PLUS)) {
			tg3_flag_set(tp, SUPPORT_MSIX);
			tp->irq_max = TG3_IRQ_MAX_VECS;
		}
	}

	tp->txq_max = 1;
	tp->rxq_max = 1;
	if (tp->irq_max > 1) {
		tp->rxq_max = TG3_RSS_MAX_NUM_QS;
		tg3_rss_init_dflt_indir_tbl(tp, TG3_RSS_MAX_NUM_QS);

		if (tg3_asic_rev(tp) == ASIC_REV_5719 ||
		    tg3_asic_rev(tp) == ASIC_REV_5720)
			tp->txq_max = tp->irq_max - 1;
	}

	if (tg3_flag(tp, 5755_PLUS) ||
	    tg3_asic_rev(tp) == ASIC_REV_5906)
		tg3_flag_set(tp, SHORT_DMA_BUG);

	if (tg3_asic_rev(tp) == ASIC_REV_5719)
		tp->dma_limit = TG3_TX_BD_DMA_MAX_4K;

	if (tg3_asic_rev(tp) == ASIC_REV_5717 ||
	    tg3_asic_rev(tp) == ASIC_REV_5719 ||
	    tg3_asic_rev(tp) == ASIC_REV_5720 ||
	    tg3_asic_rev(tp) == ASIC_REV_5762)
		tg3_flag_set(tp, LRG_PROD_RING_CAP);

	if (tg3_flag(tp, 57765_PLUS) &&
	    tg3_chip_rev_id(tp) != CHIPREV_ID_5719_A0)
		tg3_flag_set(tp, USE_JUMBO_BDFLAG);

	if (!tg3_flag(tp, 5705_PLUS) ||
	    tg3_flag(tp, 5780_CLASS) ||
	    tg3_flag(tp, USE_JUMBO_BDFLAG))
		tg3_flag_set(tp, JUMBO_CAPABLE);

	pci_read_config_dword(tp->pdev, TG3PCI_PCISTATE,
			      &pci_state_reg);

	if (pci_is_pcie(tp->pdev)) {
		u16 lnkctl;

		tg3_flag_set(tp, PCI_EXPRESS);

		pcie_capability_read_word(tp->pdev, PCI_EXP_LNKCTL, &lnkctl);
		if (lnkctl & PCI_EXP_LNKCTL_CLKREQ_EN) {
			if (tg3_asic_rev(tp) == ASIC_REV_5906) {
				tg3_flag_clear(tp, HW_TSO_2);
				tg3_flag_clear(tp, TSO_CAPABLE);
			}
			if (tg3_asic_rev(tp) == ASIC_REV_5784 ||
			    tg3_asic_rev(tp) == ASIC_REV_5761 ||
			    tg3_chip_rev_id(tp) == CHIPREV_ID_57780_A0 ||
			    tg3_chip_rev_id(tp) == CHIPREV_ID_57780_A1)
				tg3_flag_set(tp, CLKREQ_BUG);
		} else if (tg3_chip_rev_id(tp) == CHIPREV_ID_5717_A0) {
			tg3_flag_set(tp, L1PLLPD_EN);
		}
	} else if (tg3_asic_rev(tp) == ASIC_REV_5785) {
		/* BCM5785 devices are effectively PCIe devices, and should
		 * follow PCIe codepaths, but do not have a PCIe capabilities
		 * section.
		 */
		tg3_flag_set(tp, PCI_EXPRESS);
	} else if (!tg3_flag(tp, 5705_PLUS) ||
		   tg3_flag(tp, 5780_CLASS)) {
		tp->pcix_cap = pci_find_capability(tp->pdev, PCI_CAP_ID_PCIX);
		if (!tp->pcix_cap) {
			dev_err(&tp->pdev->dev,
				"Cannot find PCI-X capability, aborting\n");
			return -EIO;
		}

		if (!(pci_state_reg & PCISTATE_CONV_PCI_MODE))
			tg3_flag_set(tp, PCIX_MODE);
	}

	/* If we have an AMD 762 or VIA K8T800 chipset, write
	 * reordering to the mailbox registers done by the host
	 * controller can cause major troubles.  We read back from
	 * every mailbox register write to force the writes to be
	 * posted to the chip in order.
	 */
	if (pci_dev_present(tg3_write_reorder_chipsets) &&
	    !tg3_flag(tp, PCI_EXPRESS))
		tg3_flag_set(tp, MBOX_WRITE_REORDER);

	pci_read_config_byte(tp->pdev, PCI_CACHE_LINE_SIZE,
			     &tp->pci_cacheline_sz);
	pci_read_config_byte(tp->pdev, PCI_LATENCY_TIMER,
			     &tp->pci_lat_timer);
	if (tg3_asic_rev(tp) == ASIC_REV_5703 &&
	    tp->pci_lat_timer < 64) {
		tp->pci_lat_timer = 64;
		pci_write_config_byte(tp->pdev, PCI_LATENCY_TIMER,
				      tp->pci_lat_timer);
	}

	/* Important! -- It is critical that the PCI-X hw workaround
	 * situation is decided before the first MMIO register access.
	 */
	if (tg3_chip_rev(tp) == CHIPREV_5700_BX) {
		/* 5700 BX chips need to have their TX producer index
		 * mailboxes written twice to workaround a bug.
		 */
		tg3_flag_set(tp, TXD_MBOX_HWBUG);

		/* If we are in PCI-X mode, enable register write workaround.
		 *
		 * The workaround is to use indirect register accesses
		 * for all chip writes not to mailbox registers.
		 */
		if (tg3_flag(tp, PCIX_MODE)) {
			u32 pm_reg;

			tg3_flag_set(tp, PCIX_TARGET_HWBUG);

			/* The chip can have it's power management PCI config
			 * space registers clobbered due to this bug.
			 * So explicitly force the chip into D0 here.
			 */
			pci_read_config_dword(tp->pdev,
					      tp->pm_cap + PCI_PM_CTRL,
					      &pm_reg);
			pm_reg &= ~PCI_PM_CTRL_STATE_MASK;
			pm_reg |= PCI_PM_CTRL_PME_ENABLE | 0 /* D0 */;
			pci_write_config_dword(tp->pdev,
					       tp->pm_cap + PCI_PM_CTRL,
					       pm_reg);

			/* Also, force SERR#/PERR# in PCI command. */
			pci_read_config_word(tp->pdev, PCI_COMMAND, &pci_cmd);
			pci_cmd |= PCI_COMMAND_PARITY | PCI_COMMAND_SERR;
			pci_write_config_word(tp->pdev, PCI_COMMAND, pci_cmd);
		}
	}

	if ((pci_state_reg & PCISTATE_BUS_SPEED_HIGH) != 0)
		tg3_flag_set(tp, PCI_HIGH_SPEED);
	if ((pci_state_reg & PCISTATE_BUS_32BIT) != 0)
		tg3_flag_set(tp, PCI_32BIT);

	/* Chip-specific fixup from Broadcom driver */
	if ((tg3_chip_rev_id(tp) == CHIPREV_ID_5704_A0) &&
	    (!(pci_state_reg & PCISTATE_RETRY_SAME_DMA))) {
		pci_state_reg |= PCISTATE_RETRY_SAME_DMA;
		pci_write_config_dword(tp->pdev, TG3PCI_PCISTATE, pci_state_reg);
	}

	/* Default fast path register access methods */
	tp->read32 = tg3_read32;
	tp->write32 = tg3_write32;
	tp->read32_mbox = tg3_read32;
	tp->write32_mbox = tg3_write32;
	tp->write32_tx_mbox = tg3_write32;
	tp->write32_rx_mbox = tg3_write32;

	/* Various workaround register access methods */
	if (tg3_flag(tp, PCIX_TARGET_HWBUG))
		tp->write32 = tg3_write_indirect_reg32;
	else if (tg3_asic_rev(tp) == ASIC_REV_5701 ||
		 (tg3_flag(tp, PCI_EXPRESS) &&
		  tg3_chip_rev_id(tp) == CHIPREV_ID_5750_A0)) {
		/*
		 * Back to back register writes can cause problems on these
		 * chips, the workaround is to read back all reg writes
		 * except those to mailbox regs.
		 *
		 * See tg3_write_indirect_reg32().
		 */
		tp->write32 = tg3_write_flush_reg32;
	}

	if (tg3_flag(tp, TXD_MBOX_HWBUG) || tg3_flag(tp, MBOX_WRITE_REORDER)) {
		tp->write32_tx_mbox = tg3_write32_tx_mbox;
		if (tg3_flag(tp, MBOX_WRITE_REORDER))
			tp->write32_rx_mbox = tg3_write_flush_reg32;
	}

	if (tg3_flag(tp, ICH_WORKAROUND)) {
		tp->read32 = tg3_read_indirect_reg32;
		tp->write32 = tg3_write_indirect_reg32;
		tp->read32_mbox = tg3_read_indirect_mbox;
		tp->write32_mbox = tg3_write_indirect_mbox;
		tp->write32_tx_mbox = tg3_write_indirect_mbox;
		tp->write32_rx_mbox = tg3_write_indirect_mbox;

		iounmap(tp->regs);
		tp->regs = NULL;

		pci_read_config_word(tp->pdev, PCI_COMMAND, &pci_cmd);
		pci_cmd &= ~PCI_COMMAND_MEMORY;
		pci_write_config_word(tp->pdev, PCI_COMMAND, pci_cmd);
	}
	if (tg3_asic_rev(tp) == ASIC_REV_5906) {
		tp->read32_mbox = tg3_read32_mbox_5906;
		tp->write32_mbox = tg3_write32_mbox_5906;
		tp->write32_tx_mbox = tg3_write32_mbox_5906;
		tp->write32_rx_mbox = tg3_write32_mbox_5906;
	}

	if (tp->write32 == tg3_write_indirect_reg32 ||
	    (tg3_flag(tp, PCIX_MODE) &&
	     (tg3_asic_rev(tp) == ASIC_REV_5700 ||
	      tg3_asic_rev(tp) == ASIC_REV_5701)))
		tg3_flag_set(tp, SRAM_USE_CONFIG);

	/* The memory arbiter has to be enabled in order for SRAM accesses
	 * to succeed.  Normally on powerup the tg3 chip firmware will make
	 * sure it is enabled, but other entities such as system netboot
	 * code might disable it.
	 */
	val = tr32(MEMARB_MODE);
	tw32(MEMARB_MODE, val | MEMARB_MODE_ENABLE);

	tp->pci_fn = PCI_FUNC(tp->pdev->devfn) & 3;
	if (tg3_asic_rev(tp) == ASIC_REV_5704 ||
	    tg3_flag(tp, 5780_CLASS)) {
		if (tg3_flag(tp, PCIX_MODE)) {
			pci_read_config_dword(tp->pdev,
					      tp->pcix_cap + PCI_X_STATUS,
					      &val);
			tp->pci_fn = val & 0x7;
		}
	} else if (tg3_asic_rev(tp) == ASIC_REV_5717 ||
		   tg3_asic_rev(tp) == ASIC_REV_5719 ||
		   tg3_asic_rev(tp) == ASIC_REV_5720) {
		tg3_read_mem(tp, NIC_SRAM_CPMU_STATUS, &val);
		if ((val & NIC_SRAM_CPMUSTAT_SIG_MSK) != NIC_SRAM_CPMUSTAT_SIG)
			val = tr32(TG3_CPMU_STATUS);

		if (tg3_asic_rev(tp) == ASIC_REV_5717)
			tp->pci_fn = (val & TG3_CPMU_STATUS_FMSK_5717) ? 1 : 0;
		else
			tp->pci_fn = (val & TG3_CPMU_STATUS_FMSK_5719) >>
				     TG3_CPMU_STATUS_FSHFT_5719;
	}

	if (tg3_flag(tp, FLUSH_POSTED_WRITES)) {
		tp->write32_tx_mbox = tg3_write_flush_reg32;
		tp->write32_rx_mbox = tg3_write_flush_reg32;
	}

	/* Get eeprom hw config before calling tg3_set_power_state().
	 * In particular, the TG3_FLAG_IS_NIC flag must be
	 * determined before calling tg3_set_power_state() so that
	 * we know whether or not to switch out of Vaux power.
	 * When the flag is set, it means that GPIO1 is used for eeprom
	 * write protect and also implies that it is a LOM where GPIOs
	 * are not used to switch power.
	 */
	tg3_get_eeprom_hw_cfg(tp);

	if (tp->fw_needed && tg3_flag(tp, ENABLE_ASF)) {
		tg3_flag_clear(tp, TSO_CAPABLE);
		tg3_flag_clear(tp, TSO_BUG);
		tp->fw_needed = NULL;
	}

	if (tg3_flag(tp, ENABLE_APE)) {
		/* Allow reads and writes to the
		 * APE register and memory space.
		 */
		pci_state_reg |= PCISTATE_ALLOW_APE_CTLSPC_WR |
				 PCISTATE_ALLOW_APE_SHMEM_WR |
				 PCISTATE_ALLOW_APE_PSPACE_WR;
		pci_write_config_dword(tp->pdev, TG3PCI_PCISTATE,
				       pci_state_reg);

		tg3_ape_lock_init(tp);
	}

	/* Set up tp->grc_local_ctrl before calling
	 * tg3_pwrsrc_switch_to_vmain().  GPIO1 driven high
	 * will bring 5700's external PHY out of reset.
	 * It is also used as eeprom write protect on LOMs.
	 */
	tp->grc_local_ctrl = GRC_LCLCTRL_INT_ON_ATTN | GRC_LCLCTRL_AUTO_SEEPROM;
	if (tg3_asic_rev(tp) == ASIC_REV_5700 ||
	    tg3_flag(tp, EEPROM_WRITE_PROT))
		tp->grc_local_ctrl |= (GRC_LCLCTRL_GPIO_OE1 |
				       GRC_LCLCTRL_GPIO_OUTPUT1);
	/* Unused GPIO3 must be driven as output on 5752 because there
	 * are no pull-up resistors on unused GPIO pins.
	 */
	else if (tg3_asic_rev(tp) == ASIC_REV_5752)
		tp->grc_local_ctrl |= GRC_LCLCTRL_GPIO_OE3;

	if (tg3_asic_rev(tp) == ASIC_REV_5755 ||
	    tg3_asic_rev(tp) == ASIC_REV_57780 ||
	    tg3_flag(tp, 57765_CLASS))
		tp->grc_local_ctrl |= GRC_LCLCTRL_GPIO_UART_SEL;

	if (tp->pdev->device == PCI_DEVICE_ID_TIGON3_5761 ||
	    tp->pdev->device == TG3PCI_DEVICE_TIGON3_5761S) {
		/* Turn off the debug UART. */
		tp->grc_local_ctrl |= GRC_LCLCTRL_GPIO_UART_SEL;
		if (tg3_flag(tp, IS_NIC))
			/* Keep VMain power. */
			tp->grc_local_ctrl |= GRC_LCLCTRL_GPIO_OE0 |
					      GRC_LCLCTRL_GPIO_OUTPUT0;
	}

	if (tg3_asic_rev(tp) == ASIC_REV_5762)
		tp->grc_local_ctrl |=
			tr32(GRC_LOCAL_CTRL) & GRC_LCLCTRL_GPIO_UART_SEL;

	/* Switch out of Vaux if it is a NIC */
	tg3_pwrsrc_switch_to_vmain(tp);

	/* Derive initial jumbo mode from MTU assigned in
	 * ether_setup() via the alloc_etherdev() call
	 */
	if (tp->dev->mtu > ETH_DATA_LEN && !tg3_flag(tp, 5780_CLASS))
		tg3_flag_set(tp, JUMBO_RING_ENABLE);

	/* Determine WakeOnLan speed to use. */
	if (tg3_asic_rev(tp) == ASIC_REV_5700 ||
	    tg3_chip_rev_id(tp) == CHIPREV_ID_5701_A0 ||
	    tg3_chip_rev_id(tp) == CHIPREV_ID_5701_B0 ||
	    tg3_chip_rev_id(tp) == CHIPREV_ID_5701_B2) {
		tg3_flag_clear(tp, WOL_SPEED_100MB);
	} else {
		tg3_flag_set(tp, WOL_SPEED_100MB);
	}

	if (tg3_asic_rev(tp) == ASIC_REV_5906)
		tp->phy_flags |= TG3_PHYFLG_IS_FET;

	/* A few boards don't want Ethernet@WireSpeed phy feature */
	if (tg3_asic_rev(tp) == ASIC_REV_5700 ||
	    (tg3_asic_rev(tp) == ASIC_REV_5705 &&
	     (tg3_chip_rev_id(tp) != CHIPREV_ID_5705_A0) &&
	     (tg3_chip_rev_id(tp) != CHIPREV_ID_5705_A1)) ||
	    (tp->phy_flags & TG3_PHYFLG_IS_FET) ||
	    (tp->phy_flags & TG3_PHYFLG_ANY_SERDES))
		tp->phy_flags |= TG3_PHYFLG_NO_ETH_WIRE_SPEED;

	if (tg3_chip_rev(tp) == CHIPREV_5703_AX ||
	    tg3_chip_rev(tp) == CHIPREV_5704_AX)
		tp->phy_flags |= TG3_PHYFLG_ADC_BUG;
	if (tg3_chip_rev_id(tp) == CHIPREV_ID_5704_A0)
		tp->phy_flags |= TG3_PHYFLG_5704_A0_BUG;

	if (tg3_flag(tp, 5705_PLUS) &&
	    !(tp->phy_flags & TG3_PHYFLG_IS_FET) &&
	    tg3_asic_rev(tp) != ASIC_REV_5785 &&
	    tg3_asic_rev(tp) != ASIC_REV_57780 &&
	    !tg3_flag(tp, 57765_PLUS)) {
		if (tg3_asic_rev(tp) == ASIC_REV_5755 ||
		    tg3_asic_rev(tp) == ASIC_REV_5787 ||
		    tg3_asic_rev(tp) == ASIC_REV_5784 ||
		    tg3_asic_rev(tp) == ASIC_REV_5761) {
			if (tp->pdev->device != PCI_DEVICE_ID_TIGON3_5756 &&
			    tp->pdev->device != PCI_DEVICE_ID_TIGON3_5722)
				tp->phy_flags |= TG3_PHYFLG_JITTER_BUG;
			if (tp->pdev->device == PCI_DEVICE_ID_TIGON3_5755M)
				tp->phy_flags |= TG3_PHYFLG_ADJUST_TRIM;
		} else
			tp->phy_flags |= TG3_PHYFLG_BER_BUG;
	}

	if (tg3_asic_rev(tp) == ASIC_REV_5784 &&
	    tg3_chip_rev(tp) != CHIPREV_5784_AX) {
		tp->phy_otp = tg3_read_otp_phycfg(tp);
		if (tp->phy_otp == 0)
			tp->phy_otp = TG3_OTP_DEFAULT;
	}

	if (tg3_flag(tp, CPMU_PRESENT))
		tp->mi_mode = MAC_MI_MODE_500KHZ_CONST;
	else
		tp->mi_mode = MAC_MI_MODE_BASE;

	tp->coalesce_mode = 0;
	if (tg3_chip_rev(tp) != CHIPREV_5700_AX &&
	    tg3_chip_rev(tp) != CHIPREV_5700_BX)
		tp->coalesce_mode |= HOSTCC_MODE_32BYTE;

	/* Set these bits to enable statistics workaround. */
	if (tg3_asic_rev(tp) == ASIC_REV_5717 ||
	    tg3_chip_rev_id(tp) == CHIPREV_ID_5719_A0 ||
	    tg3_chip_rev_id(tp) == CHIPREV_ID_5720_A0) {
		tp->coalesce_mode |= HOSTCC_MODE_ATTN;
		tp->grc_mode |= GRC_MODE_IRQ_ON_FLOW_ATTN;
	}

	if (tg3_asic_rev(tp) == ASIC_REV_5785 ||
	    tg3_asic_rev(tp) == ASIC_REV_57780)
		tg3_flag_set(tp, USE_PHYLIB);

	err = tg3_mdio_init(tp);
	if (err)
		return err;

	/* Initialize data/descriptor byte/word swapping. */
	val = tr32(GRC_MODE);
	if (tg3_asic_rev(tp) == ASIC_REV_5720 ||
	    tg3_asic_rev(tp) == ASIC_REV_5762)
		val &= (GRC_MODE_BYTE_SWAP_B2HRX_DATA |
			GRC_MODE_WORD_SWAP_B2HRX_DATA |
			GRC_MODE_B2HRX_ENABLE |
			GRC_MODE_HTX2B_ENABLE |
			GRC_MODE_HOST_STACKUP);
	else
		val &= GRC_MODE_HOST_STACKUP;

	tw32(GRC_MODE, val | tp->grc_mode);

	tg3_switch_clocks(tp);

	/* Clear this out for sanity. */
	tw32(TG3PCI_MEM_WIN_BASE_ADDR, 0);

	pci_read_config_dword(tp->pdev, TG3PCI_PCISTATE,
			      &pci_state_reg);
	if ((pci_state_reg & PCISTATE_CONV_PCI_MODE) == 0 &&
	    !tg3_flag(tp, PCIX_TARGET_HWBUG)) {
		if (tg3_chip_rev_id(tp) == CHIPREV_ID_5701_A0 ||
		    tg3_chip_rev_id(tp) == CHIPREV_ID_5701_B0 ||
		    tg3_chip_rev_id(tp) == CHIPREV_ID_5701_B2 ||
		    tg3_chip_rev_id(tp) == CHIPREV_ID_5701_B5) {
			void __iomem *sram_base;

			/* Write some dummy words into the SRAM status block
			 * area, see if it reads back correctly.  If the return
			 * value is bad, force enable the PCIX workaround.
			 */
			sram_base = tp->regs + NIC_SRAM_WIN_BASE + NIC_SRAM_STATS_BLK;

			writel(0x00000000, sram_base);
			writel(0x00000000, sram_base + 4);
			writel(0xffffffff, sram_base + 4);
			if (readl(sram_base) != 0x00000000)
				tg3_flag_set(tp, PCIX_TARGET_HWBUG);
		}
	}

	udelay(50);
	tg3_nvram_init(tp);

	grc_misc_cfg = tr32(GRC_MISC_CFG);
	grc_misc_cfg &= GRC_MISC_CFG_BOARD_ID_MASK;

	if (tg3_asic_rev(tp) == ASIC_REV_5705 &&
	    (grc_misc_cfg == GRC_MISC_CFG_BOARD_ID_5788 ||
	     grc_misc_cfg == GRC_MISC_CFG_BOARD_ID_5788M))
		tg3_flag_set(tp, IS_5788);

	if (!tg3_flag(tp, IS_5788) &&
	    tg3_asic_rev(tp) != ASIC_REV_5700)
		tg3_flag_set(tp, TAGGED_STATUS);
	if (tg3_flag(tp, TAGGED_STATUS)) {
		tp->coalesce_mode |= (HOSTCC_MODE_CLRTICK_RXBD |
				      HOSTCC_MODE_CLRTICK_TXBD);

		tp->misc_host_ctrl |= MISC_HOST_CTRL_TAGGED_STATUS;
		pci_write_config_dword(tp->pdev, TG3PCI_MISC_HOST_CTRL,
				       tp->misc_host_ctrl);
	}

	/* Preserve the APE MAC_MODE bits */
	if (tg3_flag(tp, ENABLE_APE))
		tp->mac_mode = MAC_MODE_APE_TX_EN | MAC_MODE_APE_RX_EN;
	else
		tp->mac_mode = 0;

	if (tg3_10_100_only_device(tp, ent))
		tp->phy_flags |= TG3_PHYFLG_10_100_ONLY;

	err = tg3_phy_probe(tp);
	if (err) {
		dev_err(&tp->pdev->dev, "phy probe failed, err %d\n", err);
		/* ... but do not return immediately ... */
		tg3_mdio_fini(tp);
	}

	tg3_read_vpd(tp);
	tg3_read_fw_ver(tp);

	if (tp->phy_flags & TG3_PHYFLG_PHY_SERDES) {
		tp->phy_flags &= ~TG3_PHYFLG_USE_MI_INTERRUPT;
	} else {
		if (tg3_asic_rev(tp) == ASIC_REV_5700)
			tp->phy_flags |= TG3_PHYFLG_USE_MI_INTERRUPT;
		else
			tp->phy_flags &= ~TG3_PHYFLG_USE_MI_INTERRUPT;
	}

	/* 5700 {AX,BX} chips have a broken status block link
	 * change bit implementation, so we must use the
	 * status register in those cases.
	 */
	if (tg3_asic_rev(tp) == ASIC_REV_5700)
		tg3_flag_set(tp, USE_LINKCHG_REG);
	else
		tg3_flag_clear(tp, USE_LINKCHG_REG);

	/* The led_ctrl is set during tg3_phy_probe, here we might
	 * have to force the link status polling mechanism based
	 * upon subsystem IDs.
	 */
	if (tp->pdev->subsystem_vendor == PCI_VENDOR_ID_DELL &&
	    tg3_asic_rev(tp) == ASIC_REV_5701 &&
	    !(tp->phy_flags & TG3_PHYFLG_PHY_SERDES)) {
		tp->phy_flags |= TG3_PHYFLG_USE_MI_INTERRUPT;
		tg3_flag_set(tp, USE_LINKCHG_REG);
	}

	/* For all SERDES we poll the MAC status register. */
	if (tp->phy_flags & TG3_PHYFLG_PHY_SERDES)
		tg3_flag_set(tp, POLL_SERDES);
	else
		tg3_flag_clear(tp, POLL_SERDES);

	tp->rx_offset = NET_SKB_PAD + NET_IP_ALIGN;
	tp->rx_copy_thresh = TG3_RX_COPY_THRESHOLD;
	if (tg3_asic_rev(tp) == ASIC_REV_5701 &&
	    tg3_flag(tp, PCIX_MODE)) {
		tp->rx_offset = NET_SKB_PAD;
#ifndef CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
		tp->rx_copy_thresh = ~(u16)0;
#endif
	}

	tp->rx_std_ring_mask = TG3_RX_STD_RING_SIZE(tp) - 1;
	tp->rx_jmb_ring_mask = TG3_RX_JMB_RING_SIZE(tp) - 1;
	tp->rx_ret_ring_mask = tg3_rx_ret_ring_size(tp) - 1;

	tp->rx_std_max_post = tp->rx_std_ring_mask + 1;

	/* Increment the rx prod index on the rx std ring by at most
	 * 8 for these chips to workaround hw errata.
	 */
	if (tg3_asic_rev(tp) == ASIC_REV_5750 ||
	    tg3_asic_rev(tp) == ASIC_REV_5752 ||
	    tg3_asic_rev(tp) == ASIC_REV_5755)
		tp->rx_std_max_post = 8;

	if (tg3_flag(tp, ASPM_WORKAROUND))
		tp->pwrmgmt_thresh = tr32(PCIE_PWR_MGMT_THRESH) &
				     PCIE_PWR_MGMT_L1_THRESH_MSK;

	return err;
}
