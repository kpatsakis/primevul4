void atm_dev_signal_change(struct atm_dev *dev, char signal)
{
	pr_debug("%s signal=%d dev=%p number=%d dev->signal=%d\n",
		__func__, signal, dev, dev->number, dev->signal);

	/* atm driver sending invalid signal */
	WARN_ON(signal < ATM_PHY_SIG_LOST || signal > ATM_PHY_SIG_FOUND);

	if (dev->signal == signal)
		return; /* no change */

	dev->signal = signal;

	atomic_notifier_call_chain(&atm_dev_notify_chain, signal, dev);
}
