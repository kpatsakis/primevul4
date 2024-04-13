ext_compare( a, b )
    struct mime_entry* a;
    struct mime_entry* b;
    {
    return strcmp( a->ext, b->ext );
    }
