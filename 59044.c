static plist_t parse_data_node(const char **bnode, uint64_t size)
{
    plist_data_t data = plist_new_plist_data();

    data->type = PLIST_DATA;
    data->length = size;
    data->buff = (uint8_t *) malloc(sizeof(uint8_t) * size);
    memcpy(data->buff, *bnode, sizeof(uint8_t) * size);

    return node_create(NULL, data);
}
