  Buf recvEncryptedStream(
      StreamId streamId,
      folly::IOBuf& data,
      uint64_t offset = 0,
      bool eof = false) {
    PacketNum packetNum = clientNextAppDataPacketNum++;
    auto packetData = packetToBuf(createStreamPacket(
        clientConnectionId.value_or(getTestConnectionId()),
        *server->getConn().serverConnectionId,
        packetNum,
        streamId,
        data,
        0 /* cipherOverhead */,
        0 /* largestAcked */,
        folly::none /* longHeaderOverride */,
        eof,
        folly::none,
        offset));
    deliverData(packetData->clone());
    return packetData;
  }