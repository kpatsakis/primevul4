static void tg3_irq_quiesce(struct tg3 *tp)
{
	int i;

	BUG_ON(tp->irq_sync);

	tp->irq_sync = 1;
	smp_mb();

	for (i = 0; i < tp->irq_cnt; i++)
		synchronize_irq(tp->napi[i].irq_vec);
}
