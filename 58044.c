static int cp2112_wait(struct cp2112_device *dev, atomic_t *avail)
{
	int ret = 0;

	/* We have sent either a CP2112_TRANSFER_STATUS_REQUEST or a
	 * CP2112_DATA_READ_FORCE_SEND and we are waiting for the response to
	 * come in cp2112_raw_event or timeout. There will only be one of these
	 * in flight at any one time. The timeout is extremely large and is a
	 * last resort if the CP2112 has died. If we do timeout we don't expect
	 * to receive the response which would cause data races, it's not like
	 * we can do anything about it anyway.
	 */
	ret = wait_event_interruptible_timeout(dev->wait,
		atomic_read(avail), msecs_to_jiffies(RESPONSE_TIMEOUT));
	if (-ERESTARTSYS == ret)
		return ret;
	if (!ret)
		return -ETIMEDOUT;

	atomic_set(avail, 0);
	return 0;
}
