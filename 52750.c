static inline void snd_leave_user(mm_segment_t fs)
{
	set_fs(fs);
}
