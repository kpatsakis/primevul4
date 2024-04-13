static unsigned int plist_data_hash(const void* key)
{
    plist_data_t data = plist_get_data((plist_t) key);

    unsigned int hash = data->type;
    unsigned int i = 0;

    char *buff = NULL;
    unsigned int size = 0;

    switch (data->type)
    {
    case PLIST_BOOLEAN:
    case PLIST_UINT:
    case PLIST_REAL:
    case PLIST_DATE:
    case PLIST_UID:
        buff = (char *) &data->intval;	//works also for real as we use an union
        size = 8;
        break;
    case PLIST_KEY:
    case PLIST_STRING:
        buff = data->strval;
        size = data->length;
        break;
    case PLIST_DATA:
    case PLIST_ARRAY:
    case PLIST_DICT:
        buff = (char *) &key;
        size = sizeof(const void*);
        break;
    default:
        break;
    }

    hash += 5381;
    for (i = 0; i < size; buff++, i++) {
        hash = ((hash << 5) + hash) + *buff;
    }

    return hash;
}
