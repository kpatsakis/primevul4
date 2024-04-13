static void vcc_release_cb(struct sock *sk)
{
	struct atm_vcc *vcc = atm_sk(sk);

	if (vcc->release_cb)
		vcc->release_cb(vcc);
}
