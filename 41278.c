static inline u64 get_stat64(tg3_stat64_t *val)
{
       return ((u64)val->high << 32) | ((u64)val->low);
}
