static void snd_msnd_eval_dsp_msg(struct snd_msnd *chip, u16 wMessage)
{
	switch (HIBYTE(wMessage)) {
	case HIMT_PLAY_DONE: {
		if (chip->banksPlayed < 3)
			snd_printdd("%08X: HIMT_PLAY_DONE: %i\n",
				(unsigned)jiffies, LOBYTE(wMessage));

		if (chip->last_playbank == LOBYTE(wMessage)) {
			snd_printdd("chip.last_playbank == LOBYTE(wMessage)\n");
			break;
		}
		chip->banksPlayed++;

		if (test_bit(F_WRITING, &chip->flags))
			snd_msnd_DAPQ(chip, 0);

		chip->last_playbank = LOBYTE(wMessage);
		chip->playDMAPos += chip->play_period_bytes;
		if (chip->playDMAPos > chip->playLimit)
			chip->playDMAPos = 0;
		snd_pcm_period_elapsed(chip->playback_substream);

		break;
	}
	case HIMT_RECORD_DONE:
		if (chip->last_recbank == LOBYTE(wMessage))
			break;
		chip->last_recbank = LOBYTE(wMessage);
		chip->captureDMAPos += chip->capturePeriodBytes;
		if (chip->captureDMAPos > (chip->captureLimit))
			chip->captureDMAPos = 0;

		if (test_bit(F_READING, &chip->flags))
			snd_msnd_DARQ(chip, chip->last_recbank);

		snd_pcm_period_elapsed(chip->capture_substream);
		break;

	case HIMT_DSP:
		switch (LOBYTE(wMessage)) {
#ifndef MSND_CLASSIC
		case HIDSP_PLAY_UNDER:
#endif
		case HIDSP_INT_PLAY_UNDER:
			snd_printd(KERN_WARNING LOGNAME ": Play underflow %i\n",
				chip->banksPlayed);
			if (chip->banksPlayed > 2)
				clear_bit(F_WRITING, &chip->flags);
			break;

		case HIDSP_INT_RECORD_OVER:
			snd_printd(KERN_WARNING LOGNAME ": Record overflow\n");
			clear_bit(F_READING, &chip->flags);
			break;

		default:
			snd_printd(KERN_WARNING LOGNAME
				   ": DSP message %d 0x%02x\n",
				   LOBYTE(wMessage), LOBYTE(wMessage));
			break;
		}
		break;

	case HIMT_MIDI_IN_UCHAR:
		if (chip->msndmidi_mpu)
			snd_msndmidi_input_read(chip->msndmidi_mpu);
		break;

	default:
		snd_printd(KERN_WARNING LOGNAME ": HIMT message %d 0x%02x\n",
			   HIBYTE(wMessage), HIBYTE(wMessage));
		break;
	}
}
