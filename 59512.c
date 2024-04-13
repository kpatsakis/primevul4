static unsigned long klsi_105_status2linestate(const __u16 status)
{
	unsigned long res = 0;

	res =   ((status & KL5KUSB105A_DSR) ? TIOCM_DSR : 0)
	      | ((status & KL5KUSB105A_CTS) ? TIOCM_CTS : 0)
	      ;

	return res;
}
