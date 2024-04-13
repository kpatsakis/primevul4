static int snd_msnd_write_cfg(int cfg, int reg, int value)
{
	outb(reg, cfg);
	outb(value, cfg + 1);
	if (value != inb(cfg + 1)) {
		printk(KERN_ERR LOGNAME ": snd_msnd_write_cfg: I/O error\n");
		return -EIO;
	}
	return 0;
}
