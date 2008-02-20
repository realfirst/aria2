#include "MSEHandshake.h"
#include "Exception.h"
#include "Util.h"
#include "prefs.h"
#include "Socket.h"
#include "Option.h"
#include "BtRegistry.h"
#include "MockBtContext.h"
#include "FileEntry.h"
#include <cppunit/extensions/HelperMacros.h>

namespace aria2 {

class MSEHandshakeTest:public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE(MSEHandshakeTest);
  CPPUNIT_TEST(testHandshake);
  CPPUNIT_TEST_SUITE_END();
private:
  SharedHandle<MockBtContext> _btctx;

  void doHandshake(const SharedHandle<MSEHandshake>& initiator,
		   const SharedHandle<MSEHandshake>& receiver);

public:
  void setUp()
  {
    _btctx = new MockBtContext();
    unsigned char infoHash[20];
    memset(infoHash, 0, sizeof(infoHash));
    _btctx->setInfoHash(infoHash);

    BtRegistry::unregisterAll();
    BtRegistry::registerBtContext(_btctx->getInfoHashAsString(), _btctx);
  }

  void tearDown()
  {
    BtRegistry::unregisterAll();
  }

  void testHandshake();
};


CPPUNIT_TEST_SUITE_REGISTRATION(MSEHandshakeTest);

static std::pair<SocketCore*, SocketCore*> createSocketPair()
{
  SocketCore* initiatorSock = new SocketCore();

  SocketCore receiverServerSock;
  receiverServerSock.bind(0);
  receiverServerSock.beginListen();

  std::pair<std::string, int32_t> receiverAddrInfo;
  receiverServerSock.getAddrInfo(receiverAddrInfo);
  initiatorSock->establishConnection("127.0.0.1", receiverAddrInfo.second);
  initiatorSock->setBlockingMode();

  SocketCore* receiverSock = receiverServerSock.acceptConnection();
  receiverSock->setBlockingMode();

  return std::pair<SocketCore*, SocketCore*>(initiatorSock, receiverSock);
}

void MSEHandshakeTest::doHandshake(const SharedHandle<MSEHandshake>& initiator, const SharedHandle<MSEHandshake>& receiver)
{
  initiator->sendPublicKey();

  while(!receiver->receivePublicKey());
  receiver->sendPublicKey();

  while(!initiator->receivePublicKey());
  initiator->initCipher(_btctx->getInfoHash());
  initiator->sendInitiatorStep2();

  while(!receiver->findReceiverHashMarker());
  while(!receiver->receiveReceiverHashAndPadCLength());
  while(!receiver->receivePad());
  while(!receiver->receiveReceiverIALength());
  while(!receiver->receiveReceiverIA());
  receiver->sendReceiverStep2();

  while(!initiator->findInitiatorVCMarker());
  while(!initiator->receiveInitiatorCryptoSelectAndPadDLength());
  while(!initiator->receivePad());
}

static MSEHandshake* createMSEHandshake(SocketCore* socket, bool initiator, const Option* option)
{
  MSEHandshake* h = new MSEHandshake(1, socket, option);
  h->initEncryptionFacility(initiator);
  return h;
}

void MSEHandshakeTest::testHandshake()
{
  {
    Option op;
    op.put(PREF_BT_MIN_CRYPTO_LEVEL, V_PLAIN);

    std::pair<SocketCore*, SocketCore*> sockPair = createSocketPair();
    SharedHandle<MSEHandshake> initiator = createMSEHandshake(sockPair.first, true, &op);
    SharedHandle<MSEHandshake> receiver = createMSEHandshake(sockPair.second, false, &op);

    doHandshake(initiator, receiver);

    CPPUNIT_ASSERT_EQUAL(MSEHandshake::CRYPTO_PLAIN_TEXT, initiator->getNegotiatedCryptoType());
    CPPUNIT_ASSERT_EQUAL(MSEHandshake::CRYPTO_PLAIN_TEXT, receiver->getNegotiatedCryptoType());
  }
  {
    Option op;
    op.put(PREF_BT_MIN_CRYPTO_LEVEL, V_ARC4);

    std::pair<SocketCore*, SocketCore*> sockPair = createSocketPair();
    SharedHandle<MSEHandshake> initiator = createMSEHandshake(sockPair.first, true, &op);
    SharedHandle<MSEHandshake> receiver = createMSEHandshake(sockPair.second, false, &op);

    doHandshake(initiator, receiver);

    CPPUNIT_ASSERT_EQUAL(MSEHandshake::CRYPTO_ARC4, initiator->getNegotiatedCryptoType());
    CPPUNIT_ASSERT_EQUAL(MSEHandshake::CRYPTO_ARC4, receiver->getNegotiatedCryptoType());
  }
}

} // namespace aria2