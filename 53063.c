static s32 atl2_init_hw(struct atl2_hw *hw)
{
	u32 ret_val = 0;

	atl2_init_pcie(hw);

	/* Zero out the Multicast HASH table */
	/* clear the old settings from the multicast hash table */
	ATL2_WRITE_REG(hw, REG_RX_HASH_TABLE, 0);
	ATL2_WRITE_REG_ARRAY(hw, REG_RX_HASH_TABLE, 1, 0);

	atl2_init_flash_opcode(hw);

	ret_val = atl2_phy_init(hw);

	return ret_val;
}
