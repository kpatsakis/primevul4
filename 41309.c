static void tg3_detect_asic_rev(struct tg3 *tp, u32 misc_ctrl_reg)
{
	tp->pci_chip_rev_id = misc_ctrl_reg >> MISC_HOST_CTRL_CHIPREV_SHIFT;
	if (tg3_asic_rev(tp) == ASIC_REV_USE_PROD_ID_REG) {
		u32 reg;

		/* All devices that use the alternate
		 * ASIC REV location have a CPMU.
		 */
		tg3_flag_set(tp, CPMU_PRESENT);

		if (tp->pdev->device == TG3PCI_DEVICE_TIGON3_5717 ||
		    tp->pdev->device == TG3PCI_DEVICE_TIGON3_5717_C ||
		    tp->pdev->device == TG3PCI_DEVICE_TIGON3_5718 ||
		    tp->pdev->device == TG3PCI_DEVICE_TIGON3_5719 ||
		    tp->pdev->device == TG3PCI_DEVICE_TIGON3_5720 ||
		    tp->pdev->device == TG3PCI_DEVICE_TIGON3_5762 ||
		    tp->pdev->device == TG3PCI_DEVICE_TIGON3_5725 ||
		    tp->pdev->device == TG3PCI_DEVICE_TIGON3_5727)
			reg = TG3PCI_GEN2_PRODID_ASICREV;
		else if (tp->pdev->device == TG3PCI_DEVICE_TIGON3_57781 ||
			 tp->pdev->device == TG3PCI_DEVICE_TIGON3_57785 ||
			 tp->pdev->device == TG3PCI_DEVICE_TIGON3_57761 ||
			 tp->pdev->device == TG3PCI_DEVICE_TIGON3_57765 ||
			 tp->pdev->device == TG3PCI_DEVICE_TIGON3_57791 ||
			 tp->pdev->device == TG3PCI_DEVICE_TIGON3_57795 ||
			 tp->pdev->device == TG3PCI_DEVICE_TIGON3_57762 ||
			 tp->pdev->device == TG3PCI_DEVICE_TIGON3_57766 ||
			 tp->pdev->device == TG3PCI_DEVICE_TIGON3_57782 ||
			 tp->pdev->device == TG3PCI_DEVICE_TIGON3_57786)
			reg = TG3PCI_GEN15_PRODID_ASICREV;
		else
			reg = TG3PCI_PRODID_ASICREV;

		pci_read_config_dword(tp->pdev, reg, &tp->pci_chip_rev_id);
	}

	/* Wrong chip ID in 5752 A0. This code can be removed later
	 * as A0 is not in production.
	 */
	if (tg3_chip_rev_id(tp) == CHIPREV_ID_5752_A0_HW)
		tp->pci_chip_rev_id = CHIPREV_ID_5752_A0;

	if (tg3_chip_rev_id(tp) == CHIPREV_ID_5717_C0)
		tp->pci_chip_rev_id = CHIPREV_ID_5720_A0;

	if (tg3_asic_rev(tp) == ASIC_REV_5717 ||
	    tg3_asic_rev(tp) == ASIC_REV_5719 ||
	    tg3_asic_rev(tp) == ASIC_REV_5720)
		tg3_flag_set(tp, 5717_PLUS);

	if (tg3_asic_rev(tp) == ASIC_REV_57765 ||
	    tg3_asic_rev(tp) == ASIC_REV_57766)
		tg3_flag_set(tp, 57765_CLASS);

	if (tg3_flag(tp, 57765_CLASS) || tg3_flag(tp, 5717_PLUS) ||
	     tg3_asic_rev(tp) == ASIC_REV_5762)
		tg3_flag_set(tp, 57765_PLUS);

	/* Intentionally exclude ASIC_REV_5906 */
	if (tg3_asic_rev(tp) == ASIC_REV_5755 ||
	    tg3_asic_rev(tp) == ASIC_REV_5787 ||
	    tg3_asic_rev(tp) == ASIC_REV_5784 ||
	    tg3_asic_rev(tp) == ASIC_REV_5761 ||
	    tg3_asic_rev(tp) == ASIC_REV_5785 ||
	    tg3_asic_rev(tp) == ASIC_REV_57780 ||
	    tg3_flag(tp, 57765_PLUS))
		tg3_flag_set(tp, 5755_PLUS);

	if (tg3_asic_rev(tp) == ASIC_REV_5780 ||
	    tg3_asic_rev(tp) == ASIC_REV_5714)
		tg3_flag_set(tp, 5780_CLASS);

	if (tg3_asic_rev(tp) == ASIC_REV_5750 ||
	    tg3_asic_rev(tp) == ASIC_REV_5752 ||
	    tg3_asic_rev(tp) == ASIC_REV_5906 ||
	    tg3_flag(tp, 5755_PLUS) ||
	    tg3_flag(tp, 5780_CLASS))
		tg3_flag_set(tp, 5750_PLUS);

	if (tg3_asic_rev(tp) == ASIC_REV_5705 ||
	    tg3_flag(tp, 5750_PLUS))
		tg3_flag_set(tp, 5705_PLUS);
}
