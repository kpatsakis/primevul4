static inline u32 tcp_cookie_work(const u32 *ws, const int n)
{
	return ws[COOKIE_DIGEST_WORDS + ((COOKIE_MESSAGE_WORDS-1) & ws[n])];
}
