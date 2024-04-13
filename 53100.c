static int atl2_suspend(struct pci_dev *pdev, pm_message_t state)
{
	struct net_device *netdev = pci_get_drvdata(pdev);
	struct atl2_adapter *adapter = netdev_priv(netdev);
	struct atl2_hw *hw = &adapter->hw;
	u16 speed, duplex;
	u32 ctrl = 0;
	u32 wufc = adapter->wol;

#ifdef CONFIG_PM
	int retval = 0;
#endif

	netif_device_detach(netdev);

	if (netif_running(netdev)) {
		WARN_ON(test_bit(__ATL2_RESETTING, &adapter->flags));
		atl2_down(adapter);
	}

#ifdef CONFIG_PM
	retval = pci_save_state(pdev);
	if (retval)
		return retval;
#endif

	atl2_read_phy_reg(hw, MII_BMSR, (u16 *)&ctrl);
	atl2_read_phy_reg(hw, MII_BMSR, (u16 *)&ctrl);
	if (ctrl & BMSR_LSTATUS)
		wufc &= ~ATLX_WUFC_LNKC;

	if (0 != (ctrl & BMSR_LSTATUS) && 0 != wufc) {
		u32 ret_val;
		/* get current link speed & duplex */
		ret_val = atl2_get_speed_and_duplex(hw, &speed, &duplex);
		if (ret_val) {
			printk(KERN_DEBUG
				"%s: get speed&duplex error while suspend\n",
				atl2_driver_name);
			goto wol_dis;
		}

		ctrl = 0;

		/* turn on magic packet wol */
		if (wufc & ATLX_WUFC_MAG)
			ctrl |= (WOL_MAGIC_EN | WOL_MAGIC_PME_EN);

		/* ignore Link Chg event when Link is up */
		ATL2_WRITE_REG(hw, REG_WOL_CTRL, ctrl);

		/* Config MAC CTRL Register */
		ctrl = MAC_CTRL_RX_EN | MAC_CTRL_MACLP_CLK_PHY;
		if (FULL_DUPLEX == adapter->link_duplex)
			ctrl |= MAC_CTRL_DUPLX;
		ctrl |= (MAC_CTRL_ADD_CRC | MAC_CTRL_PAD);
		ctrl |= (((u32)adapter->hw.preamble_len &
			MAC_CTRL_PRMLEN_MASK) << MAC_CTRL_PRMLEN_SHIFT);
		ctrl |= (((u32)(adapter->hw.retry_buf &
			MAC_CTRL_HALF_LEFT_BUF_MASK)) <<
			MAC_CTRL_HALF_LEFT_BUF_SHIFT);
		if (wufc & ATLX_WUFC_MAG) {
			/* magic packet maybe Broadcast&multicast&Unicast */
			ctrl |= MAC_CTRL_BC_EN;
		}

		ATL2_WRITE_REG(hw, REG_MAC_CTRL, ctrl);

		/* pcie patch */
		ctrl = ATL2_READ_REG(hw, REG_PCIE_PHYMISC);
		ctrl |= PCIE_PHYMISC_FORCE_RCV_DET;
		ATL2_WRITE_REG(hw, REG_PCIE_PHYMISC, ctrl);
		ctrl = ATL2_READ_REG(hw, REG_PCIE_DLL_TX_CTRL1);
		ctrl |= PCIE_DLL_TX_CTRL1_SEL_NOR_CLK;
		ATL2_WRITE_REG(hw, REG_PCIE_DLL_TX_CTRL1, ctrl);

		pci_enable_wake(pdev, pci_choose_state(pdev, state), 1);
		goto suspend_exit;
	}

	if (0 == (ctrl&BMSR_LSTATUS) && 0 != (wufc&ATLX_WUFC_LNKC)) {
		/* link is down, so only LINK CHG WOL event enable */
		ctrl |= (WOL_LINK_CHG_EN | WOL_LINK_CHG_PME_EN);
		ATL2_WRITE_REG(hw, REG_WOL_CTRL, ctrl);
		ATL2_WRITE_REG(hw, REG_MAC_CTRL, 0);

		/* pcie patch */
		ctrl = ATL2_READ_REG(hw, REG_PCIE_PHYMISC);
		ctrl |= PCIE_PHYMISC_FORCE_RCV_DET;
		ATL2_WRITE_REG(hw, REG_PCIE_PHYMISC, ctrl);
		ctrl = ATL2_READ_REG(hw, REG_PCIE_DLL_TX_CTRL1);
		ctrl |= PCIE_DLL_TX_CTRL1_SEL_NOR_CLK;
		ATL2_WRITE_REG(hw, REG_PCIE_DLL_TX_CTRL1, ctrl);

		hw->phy_configured = false; /* re-init PHY when resume */

		pci_enable_wake(pdev, pci_choose_state(pdev, state), 1);

		goto suspend_exit;
	}

wol_dis:
	/* WOL disabled */
	ATL2_WRITE_REG(hw, REG_WOL_CTRL, 0);

	/* pcie patch */
	ctrl = ATL2_READ_REG(hw, REG_PCIE_PHYMISC);
	ctrl |= PCIE_PHYMISC_FORCE_RCV_DET;
	ATL2_WRITE_REG(hw, REG_PCIE_PHYMISC, ctrl);
	ctrl = ATL2_READ_REG(hw, REG_PCIE_DLL_TX_CTRL1);
	ctrl |= PCIE_DLL_TX_CTRL1_SEL_NOR_CLK;
	ATL2_WRITE_REG(hw, REG_PCIE_DLL_TX_CTRL1, ctrl);

	atl2_force_ps(hw);
	hw->phy_configured = false; /* re-init PHY when resume */

	pci_enable_wake(pdev, pci_choose_state(pdev, state), 0);

suspend_exit:
	if (netif_running(netdev))
		atl2_free_irq(adapter);

	pci_disable_device(pdev);

	pci_set_power_state(pdev, pci_choose_state(pdev, state));

	return 0;
}
