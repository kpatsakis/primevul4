void WebProcessProxy::didPerformClientRedirect(uint64_t pageID, const String& sourceURLString, const String& destinationURLString, uint64_t frameID)
{
    WebPageProxy* page = webPage(pageID);
    if (!page)
        return;

    if (sourceURLString.isEmpty() || destinationURLString.isEmpty())
        return;
    
    WebFrameProxy* frame = webFrame(frameID);
    MESSAGE_CHECK(frame);
    MESSAGE_CHECK(frame->page() == page);
    MESSAGE_CHECK_URL(sourceURLString);
    MESSAGE_CHECK_URL(destinationURLString);

    m_context->historyClient().didPerformClientRedirect(m_context.get(), page, sourceURLString, destinationURLString, frame);
}
