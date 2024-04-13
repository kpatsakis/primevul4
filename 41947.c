asmlinkage int vprintk(const char *fmt, va_list args)
{
	int printed_len = 0;
	int current_log_level = default_message_loglevel;
	unsigned long flags;
	int this_cpu;
	char *p;
	size_t plen;
	char special;

	boot_delay_msec();
	printk_delay();

	/* This stops the holder of console_sem just where we want him */
	local_irq_save(flags);
	this_cpu = smp_processor_id();

	/*
	 * Ouch, printk recursed into itself!
	 */
	if (unlikely(printk_cpu == this_cpu)) {
		/*
		 * If a crash is occurring during printk() on this CPU,
		 * then try to get the crash message out but make sure
		 * we can't deadlock. Otherwise just return to avoid the
		 * recursion and return - but flag the recursion so that
		 * it can be printed at the next appropriate moment:
		 */
		if (!oops_in_progress && !lockdep_recursing(current)) {
			recursion_bug = 1;
			goto out_restore_irqs;
		}
		zap_locks();
	}

	lockdep_off();
	raw_spin_lock(&logbuf_lock);
	printk_cpu = this_cpu;

	if (recursion_bug) {
		recursion_bug = 0;
		strcpy(printk_buf, recursion_bug_msg);
		printed_len = strlen(recursion_bug_msg);
	}
	/* Emit the output into the temporary buffer */
	printed_len += vscnprintf(printk_buf + printed_len,
				  sizeof(printk_buf) - printed_len, fmt, args);

	p = printk_buf;

	/* Read log level and handle special printk prefix */
	plen = log_prefix(p, &current_log_level, &special);
	if (plen) {
		p += plen;

		switch (special) {
		case 'c': /* Strip <c> KERN_CONT, continue line */
			plen = 0;
			break;
		case 'd': /* Strip <d> KERN_DEFAULT, start new line */
			plen = 0;
		default:
			if (!new_text_line) {
				emit_log_char('\n');
				new_text_line = 1;
			}
		}
	}

	/*
	 * Copy the output into log_buf. If the caller didn't provide
	 * the appropriate log prefix, we insert them here
	 */
	for (; *p; p++) {
		if (new_text_line) {
			new_text_line = 0;

			if (plen) {
				/* Copy original log prefix */
				int i;

				for (i = 0; i < plen; i++)
					emit_log_char(printk_buf[i]);
				printed_len += plen;
			} else {
				/* Add log prefix */
				emit_log_char('<');
				emit_log_char(current_log_level + '0');
				emit_log_char('>');
				printed_len += 3;
			}

			if (printk_time) {
				/* Add the current time stamp */
				char tbuf[50], *tp;
				unsigned tlen;
				unsigned long long t;
				unsigned long nanosec_rem;

				t = cpu_clock(printk_cpu);
				nanosec_rem = do_div(t, 1000000000);
				tlen = sprintf(tbuf, "[%5lu.%06lu] ",
						(unsigned long) t,
						nanosec_rem / 1000);

				for (tp = tbuf; tp < tbuf + tlen; tp++)
					emit_log_char(*tp);
				printed_len += tlen;
			}

			if (!*p)
				break;
		}

		emit_log_char(*p);
		if (*p == '\n')
			new_text_line = 1;
	}

	/*
	 * Try to acquire and then immediately release the
	 * console semaphore. The release will do all the
	 * actual magic (print out buffers, wake up klogd,
	 * etc). 
	 *
	 * The console_trylock_for_printk() function
	 * will release 'logbuf_lock' regardless of whether it
	 * actually gets the semaphore or not.
	 */
	if (console_trylock_for_printk(this_cpu))
		console_unlock();

	lockdep_on();
out_restore_irqs:
	local_irq_restore(flags);

	return printed_len;
}
