void sock_update_netprioidx(struct sock *sk, struct task_struct *task)
{
	if (in_interrupt())
		return;

	sk->sk_cgrp_prioidx = task_netprioidx(task);
}
