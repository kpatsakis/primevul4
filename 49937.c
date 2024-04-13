static enum hrtimer_restart ap_poll_timeout(struct hrtimer *unused)
{
	tasklet_schedule(&ap_tasklet);
	return HRTIMER_NORESTART;
}
