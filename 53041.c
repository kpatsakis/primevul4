static void atl2_check_options(struct atl2_adapter *adapter)
{
	int val;
	struct atl2_option opt;
	int bd = adapter->bd_number;
	if (bd >= ATL2_MAX_NIC) {
		printk(KERN_NOTICE "Warning: no configuration for board #%i\n",
			bd);
		printk(KERN_NOTICE "Using defaults for all values\n");
#ifndef module_param_array
		bd = ATL2_MAX_NIC;
#endif
	}

	/* Bytes of Transmit Memory */
	opt.type = range_option;
	opt.name = "Bytes of Transmit Memory";
	opt.err = "using default of " __MODULE_STRING(ATL2_DEFAULT_TX_MEMSIZE);
	opt.def = ATL2_DEFAULT_TX_MEMSIZE;
	opt.arg.r.min = ATL2_MIN_TX_MEMSIZE;
	opt.arg.r.max = ATL2_MAX_TX_MEMSIZE;
#ifdef module_param_array
	if (num_TxMemSize > bd) {
#endif
		val = TxMemSize[bd];
		atl2_validate_option(&val, &opt);
		adapter->txd_ring_size = ((u32) val) * 1024;
#ifdef module_param_array
	} else
		adapter->txd_ring_size = ((u32)opt.def) * 1024;
#endif
	/* txs ring size: */
	adapter->txs_ring_size = adapter->txd_ring_size / 128;
	if (adapter->txs_ring_size > 160)
		adapter->txs_ring_size = 160;

	/* Receive Memory Block Count */
	opt.type = range_option;
	opt.name = "Number of receive memory block";
	opt.err = "using default of " __MODULE_STRING(ATL2_DEFAULT_RXD_COUNT);
	opt.def = ATL2_DEFAULT_RXD_COUNT;
	opt.arg.r.min = ATL2_MIN_RXD_COUNT;
	opt.arg.r.max = ATL2_MAX_RXD_COUNT;
#ifdef module_param_array
	if (num_RxMemBlock > bd) {
#endif
		val = RxMemBlock[bd];
		atl2_validate_option(&val, &opt);
		adapter->rxd_ring_size = (u32)val;
		/* FIXME */
		/* ((u16)val)&~1; */	/* even number */
#ifdef module_param_array
	} else
		adapter->rxd_ring_size = (u32)opt.def;
#endif
	/* init RXD Flow control value */
	adapter->hw.fc_rxd_hi = (adapter->rxd_ring_size / 8) * 7;
	adapter->hw.fc_rxd_lo = (ATL2_MIN_RXD_COUNT / 8) >
		(adapter->rxd_ring_size / 12) ? (ATL2_MIN_RXD_COUNT / 8) :
		(adapter->rxd_ring_size / 12);

	/* Interrupt Moderate Timer */
	opt.type = range_option;
	opt.name = "Interrupt Moderate Timer";
	opt.err = "using default of " __MODULE_STRING(INT_MOD_DEFAULT_CNT);
	opt.def = INT_MOD_DEFAULT_CNT;
	opt.arg.r.min = INT_MOD_MIN_CNT;
	opt.arg.r.max = INT_MOD_MAX_CNT;
#ifdef module_param_array
	if (num_IntModTimer > bd) {
#endif
		val = IntModTimer[bd];
		atl2_validate_option(&val, &opt);
		adapter->imt = (u16) val;
#ifdef module_param_array
	} else
		adapter->imt = (u16)(opt.def);
#endif
	/* Flash Vendor */
	opt.type = range_option;
	opt.name = "SPI Flash Vendor";
	opt.err = "using default of " __MODULE_STRING(FLASH_VENDOR_DEFAULT);
	opt.def = FLASH_VENDOR_DEFAULT;
	opt.arg.r.min = FLASH_VENDOR_MIN;
	opt.arg.r.max = FLASH_VENDOR_MAX;
#ifdef module_param_array
	if (num_FlashVendor > bd) {
#endif
		val = FlashVendor[bd];
		atl2_validate_option(&val, &opt);
		adapter->hw.flash_vendor = (u8) val;
#ifdef module_param_array
	} else
		adapter->hw.flash_vendor = (u8)(opt.def);
#endif
	/* MediaType */
	opt.type = range_option;
	opt.name = "Speed/Duplex Selection";
	opt.err = "using default of " __MODULE_STRING(MEDIA_TYPE_AUTO_SENSOR);
	opt.def = MEDIA_TYPE_AUTO_SENSOR;
	opt.arg.r.min = MEDIA_TYPE_AUTO_SENSOR;
	opt.arg.r.max = MEDIA_TYPE_10M_HALF;
#ifdef module_param_array
	if (num_MediaType > bd) {
#endif
		val = MediaType[bd];
		atl2_validate_option(&val, &opt);
		adapter->hw.MediaType = (u16) val;
#ifdef module_param_array
	} else
		adapter->hw.MediaType = (u16)(opt.def);
#endif
}
