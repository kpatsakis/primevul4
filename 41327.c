static inline void tg3_full_unlock(struct tg3 *tp)
{
	spin_unlock_bh(&tp->lock);
}
