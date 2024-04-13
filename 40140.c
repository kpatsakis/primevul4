static inline bool skcipher_writable(struct sock *sk)
{
	return PAGE_SIZE <= skcipher_sndbuf(sk);
}
