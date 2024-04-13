static void reset_proteus(struct snd_msnd *chip)
{
	outb(HPPRORESET_ON, chip->io + HP_PROR);
	msleep(TIME_PRO_RESET);
	outb(HPPRORESET_OFF, chip->io + HP_PROR);
	msleep(TIME_PRO_RESET_DONE);
}
