local void grow_space(struct space *space)
{
    size_t more;

    /* compute next size up */
    more = grow(space->size);
    if (more == space->size)
        bail("not enough memory", "");

    /* reallocate the buffer */
    space->buf = REALLOC(space->buf, more);
    if (space->buf == NULL)
        bail("not enough memory", "");
    space->size = more;
}
