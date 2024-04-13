void WebProcessProxy::disconnect()
{
    clearConnection();

    if (m_webConnection) {
        m_webConnection->invalidate();
        m_webConnection = nullptr;
    }

    m_responsivenessTimer.stop();

    Vector<RefPtr<WebFrameProxy> > frames;
    copyValuesToVector(m_frameMap, frames);

    for (size_t i = 0, size = frames.size(); i < size; ++i)
        frames[i]->disconnect();
    m_frameMap.clear();

    if (m_downloadProxyMap)
        m_downloadProxyMap->processDidClose();

    m_context->disconnectProcess(this);
}
