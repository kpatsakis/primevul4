static void rfcomm_sock_init(struct sock *sk, struct sock *parent)
{
	struct rfcomm_pinfo *pi = rfcomm_pi(sk);

	BT_DBG("sk %p", sk);

	if (parent) {
		sk->sk_type = parent->sk_type;
		pi->dlc->defer_setup = test_bit(BT_SK_DEFER_SETUP,
						&bt_sk(parent)->flags);

		pi->sec_level = rfcomm_pi(parent)->sec_level;
		pi->role_switch = rfcomm_pi(parent)->role_switch;

		security_sk_clone(parent, sk);
	} else {
		pi->dlc->defer_setup = 0;

		pi->sec_level = BT_SECURITY_LOW;
		pi->role_switch = 0;
	}

	pi->dlc->sec_level = pi->sec_level;
	pi->dlc->role_switch = pi->role_switch;
}
