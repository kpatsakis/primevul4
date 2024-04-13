static int __xml_offset(xmlNode *xml) 
{
    int position = 0;
    xmlNode *cIter = NULL;

    for(cIter = xml; cIter->prev; cIter = cIter->prev) {
        xml_private_t *p = ((xmlNode*)cIter->prev)->_private;

        if(is_not_set(p->flags, xpf_skip)) {
            position++;
        }
    }

    return position;
}
