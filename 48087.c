dtls1_retrieve_buffered_record(SSL *s, record_pqueue *queue)
    {
    pitem *item;

    item = pqueue_pop(queue->q);
    if (item)
        {
        dtls1_copy_record(s, item);

        OPENSSL_free(item->data);
		pitem_free(item);

        return(1);
        }

    return(0);
    }
