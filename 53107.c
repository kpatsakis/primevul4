static s32 atl2_write_phy_reg(struct atl2_hw *hw, u32 reg_addr, u16 phy_data)
{
	int i;
	u32 val;

	val = ((u32)(phy_data & MDIO_DATA_MASK)) << MDIO_DATA_SHIFT |
		(reg_addr & MDIO_REG_ADDR_MASK) << MDIO_REG_ADDR_SHIFT |
		MDIO_SUP_PREAMBLE |
		MDIO_START |
		MDIO_CLK_25_4 << MDIO_CLK_SEL_SHIFT;
	ATL2_WRITE_REG(hw, REG_MDIO_CTRL, val);

	wmb();

	for (i = 0; i < MDIO_WAIT_TIMES; i++) {
		udelay(2);
		val = ATL2_READ_REG(hw, REG_MDIO_CTRL);
		if (!(val & (MDIO_START | MDIO_BUSY)))
			break;

		wmb();
	}

	if (!(val & (MDIO_START | MDIO_BUSY)))
		return 0;

	return ATLX_ERR_PHY;
}
