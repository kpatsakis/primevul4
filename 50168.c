bool sk_ns_capable(const struct sock *sk,
		   struct user_namespace *user_ns, int cap)
{
	return file_ns_capable(sk->sk_socket->file, user_ns, cap) &&
		ns_capable(user_ns, cap);
}
