static int snd_msnd_isa_match(struct device *pdev, unsigned int i)
{
	if (io[i] == SNDRV_AUTO_PORT)
		return 0;

	if (irq[i] == SNDRV_AUTO_PORT || mem[i] == SNDRV_AUTO_PORT) {
		printk(KERN_WARNING LOGNAME ": io, irq and mem must be set\n");
		return 0;
	}

#ifdef MSND_CLASSIC
	if (!(io[i] == 0x290 ||
	      io[i] == 0x260 ||
	      io[i] == 0x250 ||
	      io[i] == 0x240 ||
	      io[i] == 0x230 ||
	      io[i] == 0x220 ||
	      io[i] == 0x210 ||
	      io[i] == 0x3e0)) {
		printk(KERN_ERR LOGNAME ": \"io\" - DSP I/O base must be set "
			" to 0x210, 0x220, 0x230, 0x240, 0x250, 0x260, 0x290, "
			"or 0x3E0\n");
		return 0;
	}
#else
	if (io[i] < 0x100 || io[i] > 0x3e0 || (io[i] % 0x10) != 0) {
		printk(KERN_ERR LOGNAME
			": \"io\" - DSP I/O base must within the range 0x100 "
			"to 0x3E0 and must be evenly divisible by 0x10\n");
		return 0;
	}
#endif /* MSND_CLASSIC */

	if (!(irq[i] == 5 ||
	      irq[i] == 7 ||
	      irq[i] == 9 ||
	      irq[i] == 10 ||
	      irq[i] == 11 ||
	      irq[i] == 12)) {
		printk(KERN_ERR LOGNAME
			": \"irq\" - must be set to 5, 7, 9, 10, 11 or 12\n");
		return 0;
	}

	if (!(mem[i] == 0xb0000 ||
	      mem[i] == 0xc8000 ||
	      mem[i] == 0xd0000 ||
	      mem[i] == 0xd8000 ||
	      mem[i] == 0xe0000 ||
	      mem[i] == 0xe8000)) {
		printk(KERN_ERR LOGNAME ": \"mem\" - must be set to "
		       "0xb0000, 0xc8000, 0xd0000, 0xd8000, 0xe0000 or "
		       "0xe8000\n");
		return 0;
	}

#ifndef MSND_CLASSIC
	if (cfg[i] == SNDRV_AUTO_PORT) {
		printk(KERN_INFO LOGNAME ": Assuming PnP mode\n");
	} else if (cfg[i] != 0x250 && cfg[i] != 0x260 && cfg[i] != 0x270) {
		printk(KERN_INFO LOGNAME
			": Config port must be 0x250, 0x260 or 0x270 "
			"(or unspecified for PnP mode)\n");
		return 0;
	}
#endif /* MSND_CLASSIC */

	return 1;
}
