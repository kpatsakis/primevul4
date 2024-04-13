bool WebProcessProxy::canCreateFrame(uint64_t frameID) const
{
    return isGoodKey<WebFrameProxyMap>(frameID) && !m_frameMap.contains(frameID);
}
