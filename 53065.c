static void atl2_init_pcie(struct atl2_hw *hw)
{
    u32 value;
    value = LTSSM_TEST_MODE_DEF;
    ATL2_WRITE_REG(hw, REG_LTSSM_TEST_MODE, value);

    value = PCIE_DLL_TX_CTRL1_DEF;
    ATL2_WRITE_REG(hw, REG_PCIE_DLL_TX_CTRL1, value);
}
