get_text_data (Attr *attr)
{
    VarLenData **body = XCALLOC(VarLenData*, 2);

    body[0] = XCALLOC(VarLenData, 1);
    body[0]->len = attr->len;
    body[0]->data = CHECKED_XCALLOC(unsigned char, attr->len);
    memmove (body[0]->data, attr->buf, attr->len);
    return body;
}
