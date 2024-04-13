static int atl2_configure(struct atl2_adapter *adapter)
{
	struct atl2_hw *hw = &adapter->hw;
	u32 value;

	/* clear interrupt status */
	ATL2_WRITE_REG(&adapter->hw, REG_ISR, 0xffffffff);

	/* set MAC Address */
	value = (((u32)hw->mac_addr[2]) << 24) |
		(((u32)hw->mac_addr[3]) << 16) |
		(((u32)hw->mac_addr[4]) << 8) |
		(((u32)hw->mac_addr[5]));
	ATL2_WRITE_REG(hw, REG_MAC_STA_ADDR, value);
	value = (((u32)hw->mac_addr[0]) << 8) |
		(((u32)hw->mac_addr[1]));
	ATL2_WRITE_REG(hw, (REG_MAC_STA_ADDR+4), value);

	/* HI base address */
	ATL2_WRITE_REG(hw, REG_DESC_BASE_ADDR_HI,
		(u32)((adapter->ring_dma & 0xffffffff00000000ULL) >> 32));

	/* LO base address */
	ATL2_WRITE_REG(hw, REG_TXD_BASE_ADDR_LO,
		(u32)(adapter->txd_dma & 0x00000000ffffffffULL));
	ATL2_WRITE_REG(hw, REG_TXS_BASE_ADDR_LO,
		(u32)(adapter->txs_dma & 0x00000000ffffffffULL));
	ATL2_WRITE_REG(hw, REG_RXD_BASE_ADDR_LO,
		(u32)(adapter->rxd_dma & 0x00000000ffffffffULL));

	/* element count */
	ATL2_WRITE_REGW(hw, REG_TXD_MEM_SIZE, (u16)(adapter->txd_ring_size/4));
	ATL2_WRITE_REGW(hw, REG_TXS_MEM_SIZE, (u16)adapter->txs_ring_size);
	ATL2_WRITE_REGW(hw, REG_RXD_BUF_NUM,  (u16)adapter->rxd_ring_size);

	/* config Internal SRAM */
/*
    ATL2_WRITE_REGW(hw, REG_SRAM_TXRAM_END, sram_tx_end);
    ATL2_WRITE_REGW(hw, REG_SRAM_TXRAM_END, sram_rx_end);
*/

	/* config IPG/IFG */
	value = (((u32)hw->ipgt & MAC_IPG_IFG_IPGT_MASK) <<
		MAC_IPG_IFG_IPGT_SHIFT) |
		(((u32)hw->min_ifg & MAC_IPG_IFG_MIFG_MASK) <<
		MAC_IPG_IFG_MIFG_SHIFT) |
		(((u32)hw->ipgr1 & MAC_IPG_IFG_IPGR1_MASK) <<
		MAC_IPG_IFG_IPGR1_SHIFT)|
		(((u32)hw->ipgr2 & MAC_IPG_IFG_IPGR2_MASK) <<
		MAC_IPG_IFG_IPGR2_SHIFT);
	ATL2_WRITE_REG(hw, REG_MAC_IPG_IFG, value);

	/* config  Half-Duplex Control */
	value = ((u32)hw->lcol & MAC_HALF_DUPLX_CTRL_LCOL_MASK) |
		(((u32)hw->max_retry & MAC_HALF_DUPLX_CTRL_RETRY_MASK) <<
		MAC_HALF_DUPLX_CTRL_RETRY_SHIFT) |
		MAC_HALF_DUPLX_CTRL_EXC_DEF_EN |
		(0xa << MAC_HALF_DUPLX_CTRL_ABEBT_SHIFT) |
		(((u32)hw->jam_ipg & MAC_HALF_DUPLX_CTRL_JAMIPG_MASK) <<
		MAC_HALF_DUPLX_CTRL_JAMIPG_SHIFT);
	ATL2_WRITE_REG(hw, REG_MAC_HALF_DUPLX_CTRL, value);

	/* set Interrupt Moderator Timer */
	ATL2_WRITE_REGW(hw, REG_IRQ_MODU_TIMER_INIT, adapter->imt);
	ATL2_WRITE_REG(hw, REG_MASTER_CTRL, MASTER_CTRL_ITIMER_EN);

	/* set Interrupt Clear Timer */
	ATL2_WRITE_REGW(hw, REG_CMBDISDMA_TIMER, adapter->ict);

	/* set MTU */
	ATL2_WRITE_REG(hw, REG_MTU, adapter->netdev->mtu +
		ENET_HEADER_SIZE + VLAN_SIZE + ETHERNET_FCS_SIZE);

	/* 1590 */
	ATL2_WRITE_REG(hw, REG_TX_CUT_THRESH, 0x177);

	/* flow control */
	ATL2_WRITE_REGW(hw, REG_PAUSE_ON_TH, hw->fc_rxd_hi);
	ATL2_WRITE_REGW(hw, REG_PAUSE_OFF_TH, hw->fc_rxd_lo);

	/* Init mailbox */
	ATL2_WRITE_REGW(hw, REG_MB_TXD_WR_IDX, (u16)adapter->txd_write_ptr);
	ATL2_WRITE_REGW(hw, REG_MB_RXD_RD_IDX, (u16)adapter->rxd_read_ptr);

	/* enable DMA read/write */
	ATL2_WRITE_REGB(hw, REG_DMAR, DMAR_EN);
	ATL2_WRITE_REGB(hw, REG_DMAW, DMAW_EN);

	value = ATL2_READ_REG(&adapter->hw, REG_ISR);
	if ((value & ISR_PHY_LINKDOWN) != 0)
		value = 1; /* config failed */
	else
		value = 0;

	/* clear all interrupt status */
	ATL2_WRITE_REG(&adapter->hw, REG_ISR, 0x3fffffff);
	ATL2_WRITE_REG(&adapter->hw, REG_ISR, 0);
	return value;
}
