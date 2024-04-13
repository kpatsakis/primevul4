add_options(opt)
    option_t *opt;
{
    struct option_list *list;

    list = malloc(sizeof(*list));
    if (list == 0)
	novm("option list entry");
    list->options = opt;
    list->next = extra_options;
    extra_options = list;
}
