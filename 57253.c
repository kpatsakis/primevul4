static int snd_msnd_init_sma(struct snd_msnd *chip)
{
	static int initted;
	u16 mastVolLeft, mastVolRight;
	unsigned long flags;

#ifdef MSND_CLASSIC
	outb(chip->memid, chip->io + HP_MEMM);
#endif
	outb(HPBLKSEL_0, chip->io + HP_BLKS);
	/* Motorola 56k shared memory base */
	chip->SMA = chip->mappedbase + SMA_STRUCT_START;

	if (initted) {
		mastVolLeft = readw(chip->SMA + SMA_wCurrMastVolLeft);
		mastVolRight = readw(chip->SMA + SMA_wCurrMastVolRight);
	} else
		mastVolLeft = mastVolRight = 0;
	memset_io(chip->mappedbase, 0, 0x8000);

	/* Critical section: bank 1 access */
	spin_lock_irqsave(&chip->lock, flags);
	outb(HPBLKSEL_1, chip->io + HP_BLKS);
	memset_io(chip->mappedbase, 0, 0x8000);
	outb(HPBLKSEL_0, chip->io + HP_BLKS);
	spin_unlock_irqrestore(&chip->lock, flags);

	/* Digital audio play queue */
	chip->DAPQ = chip->mappedbase + DAPQ_OFFSET;
	snd_msnd_init_queue(chip->DAPQ, DAPQ_DATA_BUFF, DAPQ_BUFF_SIZE);

	/* Digital audio record queue */
	chip->DARQ = chip->mappedbase + DARQ_OFFSET;
	snd_msnd_init_queue(chip->DARQ, DARQ_DATA_BUFF, DARQ_BUFF_SIZE);

	/* MIDI out queue */
	chip->MODQ = chip->mappedbase + MODQ_OFFSET;
	snd_msnd_init_queue(chip->MODQ, MODQ_DATA_BUFF, MODQ_BUFF_SIZE);

	/* MIDI in queue */
	chip->MIDQ = chip->mappedbase + MIDQ_OFFSET;
	snd_msnd_init_queue(chip->MIDQ, MIDQ_DATA_BUFF, MIDQ_BUFF_SIZE);

	/* DSP -> host message queue */
	chip->DSPQ = chip->mappedbase + DSPQ_OFFSET;
	snd_msnd_init_queue(chip->DSPQ, DSPQ_DATA_BUFF, DSPQ_BUFF_SIZE);

	/* Setup some DSP values */
#ifndef MSND_CLASSIC
	writew(1, chip->SMA + SMA_wCurrPlayFormat);
	writew(chip->play_sample_size, chip->SMA + SMA_wCurrPlaySampleSize);
	writew(chip->play_channels, chip->SMA + SMA_wCurrPlayChannels);
	writew(chip->play_sample_rate, chip->SMA + SMA_wCurrPlaySampleRate);
#endif
	writew(chip->play_sample_rate, chip->SMA + SMA_wCalFreqAtoD);
	writew(mastVolLeft, chip->SMA + SMA_wCurrMastVolLeft);
	writew(mastVolRight, chip->SMA + SMA_wCurrMastVolRight);
#ifndef MSND_CLASSIC
	writel(0x00010000, chip->SMA + SMA_dwCurrPlayPitch);
	writel(0x00000001, chip->SMA + SMA_dwCurrPlayRate);
#endif
	writew(0x303, chip->SMA + SMA_wCurrInputTagBits);

	initted = 1;

	return 0;
}
