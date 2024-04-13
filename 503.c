void WebProcessProxy::didReceiveSyncMessage(CoreIPC::Connection* connection, CoreIPC::MessageID messageID, CoreIPC::MessageDecoder& decoder, OwnPtr<CoreIPC::MessageEncoder>& replyEncoder)
{
    if (m_messageReceiverMap.dispatchSyncMessage(connection, messageID, decoder, replyEncoder))
        return;

    if (m_context->dispatchSyncMessage(connection, messageID, decoder, replyEncoder))
        return;

    if (decoder.messageReceiverName() == Messages::WebProcessProxy::messageReceiverName()) {
        didReceiveSyncWebProcessProxyMessage(connection, messageID, decoder, replyEncoder);
        return;
    }

    uint64_t pageID = decoder.destinationID();
    if (!pageID)
        return;
    
    WebPageProxy* pageProxy = webPage(pageID);
    if (!pageProxy)
        return;
    
    pageProxy->didReceiveSyncMessage(connection, messageID, decoder, replyEncoder);
}
