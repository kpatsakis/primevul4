WebFrameProxy* WebProcessProxy::webFrame(uint64_t frameID) const
{
    return isGoodKey<WebFrameProxyMap>(frameID) ? m_frameMap.get(frameID).get() : 0;
}
