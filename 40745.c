static void intel_fixup_er(struct perf_event *event, int idx)
{
	event->hw.extra_reg.idx = idx;

	if (idx == EXTRA_REG_RSP_0) {
		event->hw.config &= ~INTEL_ARCH_EVENT_MASK;
		event->hw.config |= 0x01b7;
		event->hw.extra_reg.reg = MSR_OFFCORE_RSP_0;
	} else if (idx == EXTRA_REG_RSP_1) {
		event->hw.config &= ~INTEL_ARCH_EVENT_MASK;
		event->hw.config |= 0x01bb;
		event->hw.extra_reg.reg = MSR_OFFCORE_RSP_1;
	}
}
