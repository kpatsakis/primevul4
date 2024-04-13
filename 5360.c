static inline u32 vlog_alignment(u32 pos)
{
	return round_up(max(pos + BPF_LOG_MIN_ALIGNMENT / 2, BPF_LOG_ALIGNMENT),
			BPF_LOG_MIN_ALIGNMENT) - pos - 1;
}