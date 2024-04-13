static int snd_msnd_probe(struct snd_card *card)
{
	struct snd_msnd *chip = card->private_data;
	unsigned char info;
#ifndef MSND_CLASSIC
	char *xv, *rev = NULL;
	char *pin = "TB Pinnacle", *fiji = "TB Fiji";
	char *pinfiji = "TB Pinnacle/Fiji";
#endif

	if (!request_region(chip->io, DSP_NUMIO, "probing")) {
		snd_printk(KERN_ERR LOGNAME ": I/O port conflict\n");
		return -ENODEV;
	}

	if (snd_msnd_reset_dsp(chip->io, &info) < 0) {
		release_region(chip->io, DSP_NUMIO);
		return -ENODEV;
	}

#ifdef MSND_CLASSIC
	strcpy(card->shortname, "Classic/Tahiti/Monterey");
	strcpy(card->longname, "Turtle Beach Multisound");
	printk(KERN_INFO LOGNAME ": %s, "
	       "I/O 0x%lx-0x%lx, IRQ %d, memory mapped to 0x%lX-0x%lX\n",
	       card->shortname,
	       chip->io, chip->io + DSP_NUMIO - 1,
	       chip->irq,
	       chip->base, chip->base + 0x7fff);
#else
	switch (info >> 4) {
	case 0xf:
		xv = "<= 1.15";
		break;
	case 0x1:
		xv = "1.18/1.2";
		break;
	case 0x2:
		xv = "1.3";
		break;
	case 0x3:
		xv = "1.4";
		break;
	default:
		xv = "unknown";
		break;
	}

	switch (info & 0x7) {
	case 0x0:
		rev = "I";
		strcpy(card->shortname, pin);
		break;
	case 0x1:
		rev = "F";
		strcpy(card->shortname, pin);
		break;
	case 0x2:
		rev = "G";
		strcpy(card->shortname, pin);
		break;
	case 0x3:
		rev = "H";
		strcpy(card->shortname, pin);
		break;
	case 0x4:
		rev = "E";
		strcpy(card->shortname, fiji);
		break;
	case 0x5:
		rev = "C";
		strcpy(card->shortname, fiji);
		break;
	case 0x6:
		rev = "D";
		strcpy(card->shortname, fiji);
		break;
	case 0x7:
		rev = "A-B (Fiji) or A-E (Pinnacle)";
		strcpy(card->shortname, pinfiji);
		break;
	}
	strcpy(card->longname, "Turtle Beach Multisound Pinnacle");
	printk(KERN_INFO LOGNAME ": %s revision %s, Xilinx version %s, "
	       "I/O 0x%lx-0x%lx, IRQ %d, memory mapped to 0x%lX-0x%lX\n",
	       card->shortname,
	       rev, xv,
	       chip->io, chip->io + DSP_NUMIO - 1,
	       chip->irq,
	       chip->base, chip->base + 0x7fff);
#endif

	release_region(chip->io, DSP_NUMIO);
	return 0;
}
