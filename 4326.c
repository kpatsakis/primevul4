void vQueueDelete( QueueHandle_t xQueue )
{
    Queue_t * const pxQueue = xQueue;

    configASSERT( pxQueue );
    traceQUEUE_DELETE( pxQueue );

    #if ( configQUEUE_REGISTRY_SIZE > 0 )
        {
            vQueueUnregisterQueue( pxQueue );
        }
    #endif

    #if ( ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 0 ) )
        {
            /* The queue can only have been allocated dynamically - free it
             * again. */
            vPortFree( pxQueue );
        }
    #elif ( ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ) )
        {
            /* The queue could have been allocated statically or dynamically, so
             * check before attempting to free the memory. */
            if( pxQueue->ucStaticallyAllocated == ( uint8_t ) pdFALSE )
            {
                vPortFree( pxQueue );
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
    #else /* if ( ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 0 ) ) */
        {
            /* The queue must have been statically allocated, so is not going to be
             * deleted.  Avoid compiler warnings about the unused parameter. */
            ( void ) pxQueue;
        }
    #endif /* configSUPPORT_DYNAMIC_ALLOCATION */
}