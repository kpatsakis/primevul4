static void atl2_get_regs(struct net_device *netdev,
	struct ethtool_regs *regs, void *p)
{
	struct atl2_adapter *adapter = netdev_priv(netdev);
	struct atl2_hw *hw = &adapter->hw;
	u32 *regs_buff = p;
	u16 phy_data;

	memset(p, 0, sizeof(u32) * ATL2_REGS_LEN);

	regs->version = (1 << 24) | (hw->revision_id << 16) | hw->device_id;

	regs_buff[0]  = ATL2_READ_REG(hw, REG_VPD_CAP);
	regs_buff[1]  = ATL2_READ_REG(hw, REG_SPI_FLASH_CTRL);
	regs_buff[2]  = ATL2_READ_REG(hw, REG_SPI_FLASH_CONFIG);
	regs_buff[3]  = ATL2_READ_REG(hw, REG_TWSI_CTRL);
	regs_buff[4]  = ATL2_READ_REG(hw, REG_PCIE_DEV_MISC_CTRL);
	regs_buff[5]  = ATL2_READ_REG(hw, REG_MASTER_CTRL);
	regs_buff[6]  = ATL2_READ_REG(hw, REG_MANUAL_TIMER_INIT);
	regs_buff[7]  = ATL2_READ_REG(hw, REG_IRQ_MODU_TIMER_INIT);
	regs_buff[8]  = ATL2_READ_REG(hw, REG_PHY_ENABLE);
	regs_buff[9]  = ATL2_READ_REG(hw, REG_CMBDISDMA_TIMER);
	regs_buff[10] = ATL2_READ_REG(hw, REG_IDLE_STATUS);
	regs_buff[11] = ATL2_READ_REG(hw, REG_MDIO_CTRL);
	regs_buff[12] = ATL2_READ_REG(hw, REG_SERDES_LOCK);
	regs_buff[13] = ATL2_READ_REG(hw, REG_MAC_CTRL);
	regs_buff[14] = ATL2_READ_REG(hw, REG_MAC_IPG_IFG);
	regs_buff[15] = ATL2_READ_REG(hw, REG_MAC_STA_ADDR);
	regs_buff[16] = ATL2_READ_REG(hw, REG_MAC_STA_ADDR+4);
	regs_buff[17] = ATL2_READ_REG(hw, REG_RX_HASH_TABLE);
	regs_buff[18] = ATL2_READ_REG(hw, REG_RX_HASH_TABLE+4);
	regs_buff[19] = ATL2_READ_REG(hw, REG_MAC_HALF_DUPLX_CTRL);
	regs_buff[20] = ATL2_READ_REG(hw, REG_MTU);
	regs_buff[21] = ATL2_READ_REG(hw, REG_WOL_CTRL);
	regs_buff[22] = ATL2_READ_REG(hw, REG_SRAM_TXRAM_END);
	regs_buff[23] = ATL2_READ_REG(hw, REG_DESC_BASE_ADDR_HI);
	regs_buff[24] = ATL2_READ_REG(hw, REG_TXD_BASE_ADDR_LO);
	regs_buff[25] = ATL2_READ_REG(hw, REG_TXD_MEM_SIZE);
	regs_buff[26] = ATL2_READ_REG(hw, REG_TXS_BASE_ADDR_LO);
	regs_buff[27] = ATL2_READ_REG(hw, REG_TXS_MEM_SIZE);
	regs_buff[28] = ATL2_READ_REG(hw, REG_RXD_BASE_ADDR_LO);
	regs_buff[29] = ATL2_READ_REG(hw, REG_RXD_BUF_NUM);
	regs_buff[30] = ATL2_READ_REG(hw, REG_DMAR);
	regs_buff[31] = ATL2_READ_REG(hw, REG_TX_CUT_THRESH);
	regs_buff[32] = ATL2_READ_REG(hw, REG_DMAW);
	regs_buff[33] = ATL2_READ_REG(hw, REG_PAUSE_ON_TH);
	regs_buff[34] = ATL2_READ_REG(hw, REG_PAUSE_OFF_TH);
	regs_buff[35] = ATL2_READ_REG(hw, REG_MB_TXD_WR_IDX);
	regs_buff[36] = ATL2_READ_REG(hw, REG_MB_RXD_RD_IDX);
	regs_buff[38] = ATL2_READ_REG(hw, REG_ISR);
	regs_buff[39] = ATL2_READ_REG(hw, REG_IMR);

	atl2_read_phy_reg(hw, MII_BMCR, &phy_data);
	regs_buff[40] = (u32)phy_data;
	atl2_read_phy_reg(hw, MII_BMSR, &phy_data);
	regs_buff[41] = (u32)phy_data;
}
