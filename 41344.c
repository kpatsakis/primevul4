static void tg3_get_eeprom_hw_cfg(struct tg3 *tp)
{
	u32 val;

	tp->phy_id = TG3_PHY_ID_INVALID;
	tp->led_ctrl = LED_CTRL_MODE_PHY_1;

	/* Assume an onboard device and WOL capable by default.  */
	tg3_flag_set(tp, EEPROM_WRITE_PROT);
	tg3_flag_set(tp, WOL_CAP);

	if (tg3_asic_rev(tp) == ASIC_REV_5906) {
		if (!(tr32(PCIE_TRANSACTION_CFG) & PCIE_TRANS_CFG_LOM)) {
			tg3_flag_clear(tp, EEPROM_WRITE_PROT);
			tg3_flag_set(tp, IS_NIC);
		}
		val = tr32(VCPU_CFGSHDW);
		if (val & VCPU_CFGSHDW_ASPM_DBNC)
			tg3_flag_set(tp, ASPM_WORKAROUND);
		if ((val & VCPU_CFGSHDW_WOL_ENABLE) &&
		    (val & VCPU_CFGSHDW_WOL_MAGPKT)) {
			tg3_flag_set(tp, WOL_ENABLE);
			device_set_wakeup_enable(&tp->pdev->dev, true);
		}
		goto done;
	}

	tg3_read_mem(tp, NIC_SRAM_DATA_SIG, &val);
	if (val == NIC_SRAM_DATA_SIG_MAGIC) {
		u32 nic_cfg, led_cfg;
		u32 nic_phy_id, ver, cfg2 = 0, cfg4 = 0, eeprom_phy_id;
		int eeprom_phy_serdes = 0;

		tg3_read_mem(tp, NIC_SRAM_DATA_CFG, &nic_cfg);
		tp->nic_sram_data_cfg = nic_cfg;

		tg3_read_mem(tp, NIC_SRAM_DATA_VER, &ver);
		ver >>= NIC_SRAM_DATA_VER_SHIFT;
		if (tg3_asic_rev(tp) != ASIC_REV_5700 &&
		    tg3_asic_rev(tp) != ASIC_REV_5701 &&
		    tg3_asic_rev(tp) != ASIC_REV_5703 &&
		    (ver > 0) && (ver < 0x100))
			tg3_read_mem(tp, NIC_SRAM_DATA_CFG_2, &cfg2);

		if (tg3_asic_rev(tp) == ASIC_REV_5785)
			tg3_read_mem(tp, NIC_SRAM_DATA_CFG_4, &cfg4);

		if ((nic_cfg & NIC_SRAM_DATA_CFG_PHY_TYPE_MASK) ==
		    NIC_SRAM_DATA_CFG_PHY_TYPE_FIBER)
			eeprom_phy_serdes = 1;

		tg3_read_mem(tp, NIC_SRAM_DATA_PHY_ID, &nic_phy_id);
		if (nic_phy_id != 0) {
			u32 id1 = nic_phy_id & NIC_SRAM_DATA_PHY_ID1_MASK;
			u32 id2 = nic_phy_id & NIC_SRAM_DATA_PHY_ID2_MASK;

			eeprom_phy_id  = (id1 >> 16) << 10;
			eeprom_phy_id |= (id2 & 0xfc00) << 16;
			eeprom_phy_id |= (id2 & 0x03ff) <<  0;
		} else
			eeprom_phy_id = 0;

		tp->phy_id = eeprom_phy_id;
		if (eeprom_phy_serdes) {
			if (!tg3_flag(tp, 5705_PLUS))
				tp->phy_flags |= TG3_PHYFLG_PHY_SERDES;
			else
				tp->phy_flags |= TG3_PHYFLG_MII_SERDES;
		}

		if (tg3_flag(tp, 5750_PLUS))
			led_cfg = cfg2 & (NIC_SRAM_DATA_CFG_LED_MODE_MASK |
				    SHASTA_EXT_LED_MODE_MASK);
		else
			led_cfg = nic_cfg & NIC_SRAM_DATA_CFG_LED_MODE_MASK;

		switch (led_cfg) {
		default:
		case NIC_SRAM_DATA_CFG_LED_MODE_PHY_1:
			tp->led_ctrl = LED_CTRL_MODE_PHY_1;
			break;

		case NIC_SRAM_DATA_CFG_LED_MODE_PHY_2:
			tp->led_ctrl = LED_CTRL_MODE_PHY_2;
			break;

		case NIC_SRAM_DATA_CFG_LED_MODE_MAC:
			tp->led_ctrl = LED_CTRL_MODE_MAC;

			/* Default to PHY_1_MODE if 0 (MAC_MODE) is
			 * read on some older 5700/5701 bootcode.
			 */
			if (tg3_asic_rev(tp) == ASIC_REV_5700 ||
			    tg3_asic_rev(tp) == ASIC_REV_5701)
				tp->led_ctrl = LED_CTRL_MODE_PHY_1;

			break;

		case SHASTA_EXT_LED_SHARED:
			tp->led_ctrl = LED_CTRL_MODE_SHARED;
			if (tg3_chip_rev_id(tp) != CHIPREV_ID_5750_A0 &&
			    tg3_chip_rev_id(tp) != CHIPREV_ID_5750_A1)
				tp->led_ctrl |= (LED_CTRL_MODE_PHY_1 |
						 LED_CTRL_MODE_PHY_2);
			break;

		case SHASTA_EXT_LED_MAC:
			tp->led_ctrl = LED_CTRL_MODE_SHASTA_MAC;
			break;

		case SHASTA_EXT_LED_COMBO:
			tp->led_ctrl = LED_CTRL_MODE_COMBO;
			if (tg3_chip_rev_id(tp) != CHIPREV_ID_5750_A0)
				tp->led_ctrl |= (LED_CTRL_MODE_PHY_1 |
						 LED_CTRL_MODE_PHY_2);
			break;

		}

		if ((tg3_asic_rev(tp) == ASIC_REV_5700 ||
		     tg3_asic_rev(tp) == ASIC_REV_5701) &&
		    tp->pdev->subsystem_vendor == PCI_VENDOR_ID_DELL)
			tp->led_ctrl = LED_CTRL_MODE_PHY_2;

		if (tg3_chip_rev(tp) == CHIPREV_5784_AX)
			tp->led_ctrl = LED_CTRL_MODE_PHY_1;

		if (nic_cfg & NIC_SRAM_DATA_CFG_EEPROM_WP) {
			tg3_flag_set(tp, EEPROM_WRITE_PROT);
			if ((tp->pdev->subsystem_vendor ==
			     PCI_VENDOR_ID_ARIMA) &&
			    (tp->pdev->subsystem_device == 0x205a ||
			     tp->pdev->subsystem_device == 0x2063))
				tg3_flag_clear(tp, EEPROM_WRITE_PROT);
		} else {
			tg3_flag_clear(tp, EEPROM_WRITE_PROT);
			tg3_flag_set(tp, IS_NIC);
		}

		if (nic_cfg & NIC_SRAM_DATA_CFG_ASF_ENABLE) {
			tg3_flag_set(tp, ENABLE_ASF);
			if (tg3_flag(tp, 5750_PLUS))
				tg3_flag_set(tp, ASF_NEW_HANDSHAKE);
		}

		if ((nic_cfg & NIC_SRAM_DATA_CFG_APE_ENABLE) &&
		    tg3_flag(tp, 5750_PLUS))
			tg3_flag_set(tp, ENABLE_APE);

		if (tp->phy_flags & TG3_PHYFLG_ANY_SERDES &&
		    !(nic_cfg & NIC_SRAM_DATA_CFG_FIBER_WOL))
			tg3_flag_clear(tp, WOL_CAP);

		if (tg3_flag(tp, WOL_CAP) &&
		    (nic_cfg & NIC_SRAM_DATA_CFG_WOL_ENABLE)) {
			tg3_flag_set(tp, WOL_ENABLE);
			device_set_wakeup_enable(&tp->pdev->dev, true);
		}

		if (cfg2 & (1 << 17))
			tp->phy_flags |= TG3_PHYFLG_CAPACITIVE_COUPLING;

		/* serdes signal pre-emphasis in register 0x590 set by */
		/* bootcode if bit 18 is set */
		if (cfg2 & (1 << 18))
			tp->phy_flags |= TG3_PHYFLG_SERDES_PREEMPHASIS;

		if ((tg3_flag(tp, 57765_PLUS) ||
		     (tg3_asic_rev(tp) == ASIC_REV_5784 &&
		      tg3_chip_rev(tp) != CHIPREV_5784_AX)) &&
		    (cfg2 & NIC_SRAM_DATA_CFG_2_APD_EN))
			tp->phy_flags |= TG3_PHYFLG_ENABLE_APD;

		if (tg3_flag(tp, PCI_EXPRESS) &&
		    tg3_asic_rev(tp) != ASIC_REV_5785 &&
		    !tg3_flag(tp, 57765_PLUS)) {
			u32 cfg3;

			tg3_read_mem(tp, NIC_SRAM_DATA_CFG_3, &cfg3);
			if (cfg3 & NIC_SRAM_ASPM_DEBOUNCE)
				tg3_flag_set(tp, ASPM_WORKAROUND);
		}

		if (cfg4 & NIC_SRAM_RGMII_INBAND_DISABLE)
			tg3_flag_set(tp, RGMII_INBAND_DISABLE);
		if (cfg4 & NIC_SRAM_RGMII_EXT_IBND_RX_EN)
			tg3_flag_set(tp, RGMII_EXT_IBND_RX_EN);
		if (cfg4 & NIC_SRAM_RGMII_EXT_IBND_TX_EN)
			tg3_flag_set(tp, RGMII_EXT_IBND_TX_EN);
	}
done:
	if (tg3_flag(tp, WOL_CAP))
		device_set_wakeup_enable(&tp->pdev->dev,
					 tg3_flag(tp, WOL_ENABLE));
	else
		device_set_wakeup_capable(&tp->pdev->dev, false);
}
