static plist_t parse_date_node(const char **bnode, uint8_t size)
{
    plist_t node = parse_real_node(bnode, size);
    plist_data_t data = plist_get_data(node);

    data->type = PLIST_DATE;

    return node;
}
