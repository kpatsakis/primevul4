static s32 atl2_phy_init(struct atl2_hw *hw)
{
	s32 ret_val;
	u16 phy_val;

	if (hw->phy_configured)
		return 0;

	/* Enable PHY */
	ATL2_WRITE_REGW(hw, REG_PHY_ENABLE, 1);
	ATL2_WRITE_FLUSH(hw);
	msleep(1);

	/* check if the PHY is in powersaving mode */
	atl2_write_phy_reg(hw, MII_DBG_ADDR, 0);
	atl2_read_phy_reg(hw, MII_DBG_DATA, &phy_val);

	/* 024E / 124E 0r 0274 / 1274 ? */
	if (phy_val & 0x1000) {
		phy_val &= ~0x1000;
		atl2_write_phy_reg(hw, MII_DBG_DATA, phy_val);
	}

	msleep(1);

	/*Enable PHY LinkChange Interrupt */
	ret_val = atl2_write_phy_reg(hw, 18, 0xC00);
	if (ret_val)
		return ret_val;

	/* setup AutoNeg parameters */
	ret_val = atl2_phy_setup_autoneg_adv(hw);
	if (ret_val)
		return ret_val;

	/* SW.Reset & En-Auto-Neg to restart Auto-Neg */
	ret_val = atl2_phy_commit(hw);
	if (ret_val)
		return ret_val;

	hw->phy_configured = true;

	return ret_val;
}
