void sock_diag_save_cookie(void *sk, __u32 *cookie)
{
	cookie[0] = (u32)(unsigned long)sk;
	cookie[1] = (u32)(((unsigned long)sk >> 31) >> 1);
}
