static void comedi_set_subdevice_runflags(struct comedi_subdevice *s,
					  unsigned mask, unsigned bits)
{
	unsigned long flags;

	spin_lock_irqsave(&s->spin_lock, flags);
	s->runflags &= ~mask;
	s->runflags |= (bits & mask);
	spin_unlock_irqrestore(&s->spin_lock, flags);
}
