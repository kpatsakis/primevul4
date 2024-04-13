static int addmbox_to_list(const mbentry_t *mbentry, void *rock)
{
    strarray_t *list = (strarray_t *)rock;
    strarray_append(list, mbentry->name);
    return 0;
}
