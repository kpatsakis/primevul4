static u32 tg3_calc_dma_bndry(struct tg3 *tp, u32 val)
{
	int cacheline_size;
	u8 byte;
	int goal;

	pci_read_config_byte(tp->pdev, PCI_CACHE_LINE_SIZE, &byte);
	if (byte == 0)
		cacheline_size = 1024;
	else
		cacheline_size = (int) byte * 4;

	/* On 5703 and later chips, the boundary bits have no
	 * effect.
	 */
	if (tg3_asic_rev(tp) != ASIC_REV_5700 &&
	    tg3_asic_rev(tp) != ASIC_REV_5701 &&
	    !tg3_flag(tp, PCI_EXPRESS))
		goto out;

#if defined(CONFIG_PPC64) || defined(CONFIG_IA64) || defined(CONFIG_PARISC)
	goal = BOUNDARY_MULTI_CACHELINE;
#else
#if defined(CONFIG_SPARC64) || defined(CONFIG_ALPHA)
	goal = BOUNDARY_SINGLE_CACHELINE;
#else
	goal = 0;
#endif
#endif

	if (tg3_flag(tp, 57765_PLUS)) {
		val = goal ? 0 : DMA_RWCTRL_DIS_CACHE_ALIGNMENT;
		goto out;
	}

	if (!goal)
		goto out;

	/* PCI controllers on most RISC systems tend to disconnect
	 * when a device tries to burst across a cache-line boundary.
	 * Therefore, letting tg3 do so just wastes PCI bandwidth.
	 *
	 * Unfortunately, for PCI-E there are only limited
	 * write-side controls for this, and thus for reads
	 * we will still get the disconnects.  We'll also waste
	 * these PCI cycles for both read and write for chips
	 * other than 5700 and 5701 which do not implement the
	 * boundary bits.
	 */
	if (tg3_flag(tp, PCIX_MODE) && !tg3_flag(tp, PCI_EXPRESS)) {
		switch (cacheline_size) {
		case 16:
		case 32:
		case 64:
		case 128:
			if (goal == BOUNDARY_SINGLE_CACHELINE) {
				val |= (DMA_RWCTRL_READ_BNDRY_128_PCIX |
					DMA_RWCTRL_WRITE_BNDRY_128_PCIX);
			} else {
				val |= (DMA_RWCTRL_READ_BNDRY_384_PCIX |
					DMA_RWCTRL_WRITE_BNDRY_384_PCIX);
			}
			break;

		case 256:
			val |= (DMA_RWCTRL_READ_BNDRY_256_PCIX |
				DMA_RWCTRL_WRITE_BNDRY_256_PCIX);
			break;

		default:
			val |= (DMA_RWCTRL_READ_BNDRY_384_PCIX |
				DMA_RWCTRL_WRITE_BNDRY_384_PCIX);
			break;
		}
	} else if (tg3_flag(tp, PCI_EXPRESS)) {
		switch (cacheline_size) {
		case 16:
		case 32:
		case 64:
			if (goal == BOUNDARY_SINGLE_CACHELINE) {
				val &= ~DMA_RWCTRL_WRITE_BNDRY_DISAB_PCIE;
				val |= DMA_RWCTRL_WRITE_BNDRY_64_PCIE;
				break;
			}
			/* fallthrough */
		case 128:
		default:
			val &= ~DMA_RWCTRL_WRITE_BNDRY_DISAB_PCIE;
			val |= DMA_RWCTRL_WRITE_BNDRY_128_PCIE;
			break;
		}
	} else {
		switch (cacheline_size) {
		case 16:
			if (goal == BOUNDARY_SINGLE_CACHELINE) {
				val |= (DMA_RWCTRL_READ_BNDRY_16 |
					DMA_RWCTRL_WRITE_BNDRY_16);
				break;
			}
			/* fallthrough */
		case 32:
			if (goal == BOUNDARY_SINGLE_CACHELINE) {
				val |= (DMA_RWCTRL_READ_BNDRY_32 |
					DMA_RWCTRL_WRITE_BNDRY_32);
				break;
			}
			/* fallthrough */
		case 64:
			if (goal == BOUNDARY_SINGLE_CACHELINE) {
				val |= (DMA_RWCTRL_READ_BNDRY_64 |
					DMA_RWCTRL_WRITE_BNDRY_64);
				break;
			}
			/* fallthrough */
		case 128:
			if (goal == BOUNDARY_SINGLE_CACHELINE) {
				val |= (DMA_RWCTRL_READ_BNDRY_128 |
					DMA_RWCTRL_WRITE_BNDRY_128);
				break;
			}
			/* fallthrough */
		case 256:
			val |= (DMA_RWCTRL_READ_BNDRY_256 |
				DMA_RWCTRL_WRITE_BNDRY_256);
			break;
		case 512:
			val |= (DMA_RWCTRL_READ_BNDRY_512 |
				DMA_RWCTRL_WRITE_BNDRY_512);
			break;
		case 1024:
		default:
			val |= (DMA_RWCTRL_READ_BNDRY_1024 |
				DMA_RWCTRL_WRITE_BNDRY_1024);
			break;
		}
	}

out:
	return val;
}
