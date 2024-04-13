Vector<WebPageProxy*> WebProcessProxy::pages() const
{
    Vector<WebPageProxy*> result;
    copyValuesToVector(m_pageMap, result);
    return result;
}
