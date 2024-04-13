static void reschedule_timeout(int drive, const char *message)
{
	unsigned long flags;

	spin_lock_irqsave(&floppy_lock, flags);
	__reschedule_timeout(drive, message);
	spin_unlock_irqrestore(&floppy_lock, flags);
}
