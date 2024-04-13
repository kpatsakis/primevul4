static int rock_check_overflow(struct rock_state *rs, int sig)
{
	int len;

	switch (sig) {
	case SIG('S', 'P'):
		len = sizeof(struct SU_SP_s);
		break;
	case SIG('C', 'E'):
		len = sizeof(struct SU_CE_s);
		break;
	case SIG('E', 'R'):
		len = sizeof(struct SU_ER_s);
		break;
	case SIG('R', 'R'):
		len = sizeof(struct RR_RR_s);
		break;
	case SIG('P', 'X'):
		len = sizeof(struct RR_PX_s);
		break;
	case SIG('P', 'N'):
		len = sizeof(struct RR_PN_s);
		break;
	case SIG('S', 'L'):
		len = sizeof(struct RR_SL_s);
		break;
	case SIG('N', 'M'):
		len = sizeof(struct RR_NM_s);
		break;
	case SIG('C', 'L'):
		len = sizeof(struct RR_CL_s);
		break;
	case SIG('P', 'L'):
		len = sizeof(struct RR_PL_s);
		break;
	case SIG('T', 'F'):
		len = sizeof(struct RR_TF_s);
		break;
	case SIG('Z', 'F'):
		len = sizeof(struct RR_ZF_s);
		break;
	default:
		len = 0;
		break;
	}
	len += offsetof(struct rock_ridge, u);
	if (len > rs->len) {
		printk(KERN_NOTICE "rock: directory entry would overflow "
				"storage\n");
		printk(KERN_NOTICE "rock: sig=0x%02x, size=%d, remaining=%d\n",
				sig, len, rs->len);
		return -EIO;
	}
	return 0;
}
