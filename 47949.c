local void drop_space(struct space *space)
{
    int use;
    struct pool *pool;

    possess(space->use);
    use = peek_lock(space->use);
    assert(use != 0);
    if (use == 1) {
        pool = space->pool;
        possess(pool->have);
        space->next = pool->head;
        pool->head = space;
        twist(pool->have, BY, +1);
    }
    twist(space->use, BY, -1);
}
