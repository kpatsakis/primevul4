    QueueSetHandle_t xQueueCreateSet( const UBaseType_t uxEventQueueLength )
    {
        QueueSetHandle_t pxQueue;

        pxQueue = xQueueGenericCreate( uxEventQueueLength, ( UBaseType_t ) sizeof( Queue_t * ), queueQUEUE_TYPE_SET );

        return pxQueue;
    }