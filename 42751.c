static inline void iriap_start_watchdog_timer(struct iriap_cb *self,
					      int timeout)
{
	irda_start_timer(&self->watchdog_timer, timeout, self,
			 iriap_watchdog_timer_expired);
}
