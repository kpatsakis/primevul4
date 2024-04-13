static void tcp_get_info_chrono_stats(const struct tcp_sock *tp,
				      struct tcp_info *info)
{
	u64 stats[__TCP_CHRONO_MAX], total = 0;
	enum tcp_chrono i;

	for (i = TCP_CHRONO_BUSY; i < __TCP_CHRONO_MAX; ++i) {
		stats[i] = tp->chrono_stat[i - 1];
		if (i == tp->chrono_type)
			stats[i] += tcp_time_stamp - tp->chrono_start;
		stats[i] *= USEC_PER_SEC / HZ;
		total += stats[i];
	}

	info->tcpi_busy_time = total;
	info->tcpi_rwnd_limited = stats[TCP_CHRONO_RWND_LIMITED];
	info->tcpi_sndbuf_limited = stats[TCP_CHRONO_SNDBUF_LIMITED];
}
