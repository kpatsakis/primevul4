static void atl2_init_flash_opcode(struct atl2_hw *hw)
{
	if (hw->flash_vendor >= ARRAY_SIZE(flash_table))
		hw->flash_vendor = 0; /* ATMEL */

	/* Init OP table */
	ATL2_WRITE_REGB(hw, REG_SPI_FLASH_OP_PROGRAM,
		flash_table[hw->flash_vendor].cmdPROGRAM);
	ATL2_WRITE_REGB(hw, REG_SPI_FLASH_OP_SC_ERASE,
		flash_table[hw->flash_vendor].cmdSECTOR_ERASE);
	ATL2_WRITE_REGB(hw, REG_SPI_FLASH_OP_CHIP_ERASE,
		flash_table[hw->flash_vendor].cmdCHIP_ERASE);
	ATL2_WRITE_REGB(hw, REG_SPI_FLASH_OP_RDID,
		flash_table[hw->flash_vendor].cmdRDID);
	ATL2_WRITE_REGB(hw, REG_SPI_FLASH_OP_WREN,
		flash_table[hw->flash_vendor].cmdWREN);
	ATL2_WRITE_REGB(hw, REG_SPI_FLASH_OP_RDSR,
		flash_table[hw->flash_vendor].cmdRDSR);
	ATL2_WRITE_REGB(hw, REG_SPI_FLASH_OP_WRSR,
		flash_table[hw->flash_vendor].cmdWRSR);
	ATL2_WRITE_REGB(hw, REG_SPI_FLASH_OP_READ,
		flash_table[hw->flash_vendor].cmdREAD);
}
