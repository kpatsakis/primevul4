local int free_pool(struct pool *pool)
{
    int count;
    struct space *space;

    possess(pool->have);
    count = 0;
    while ((space = pool->head) != NULL) {
        pool->head = space->next;
        FREE(space->buf);
        free_lock(space->use);
        FREE(space);
        count++;
    }
    assert(count == pool->made);
    release(pool->have);
    free_lock(pool->have);
    return count;
}
