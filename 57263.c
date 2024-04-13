static int snd_msnd_reset_dsp(long io, unsigned char *info)
{
	int timeout = 100;

	outb(HPDSPRESET_ON, io + HP_DSPR);
	msleep(1);
#ifndef MSND_CLASSIC
	if (info)
		*info = inb(io + HP_INFO);
#endif
	outb(HPDSPRESET_OFF, io + HP_DSPR);
	msleep(1);
	while (timeout-- > 0) {
		if (inb(io + HP_CVR) == HP_CVR_DEF)
			return 0;
		msleep(1);
	}
	snd_printk(KERN_ERR LOGNAME ": Cannot reset DSP\n");

	return -EIO;
}
