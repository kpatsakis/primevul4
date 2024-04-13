local void new_pool(struct pool *pool, size_t size, int limit)
{
    pool->have = new_lock(0);
    pool->head = NULL;
    pool->size = size;
    pool->limit = limit;
    pool->made = 0;
}
