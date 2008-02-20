/* <!-- copyright */
/*
 * aria2 - The high speed download utility
 *
 * Copyright (C) 2006 Tatsuhiro Tsujikawa
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */
/* copyright --> */
#ifndef _D_DHT_ANNOUNCE_PEER_MESSAGE_H_
#define _D_DHT_ANNOUNCE_PEER_MESSAGE_H_

#include "DHTQueryMessage.h"
#include "DHTConstants.h"

namespace aria2 {

class DHTPeerAnnounceStorage;
class DHTTokenTracker;

class DHTAnnouncePeerMessage:public DHTQueryMessage {
private:
  std::string _token;

  unsigned char _infoHash[DHT_ID_LENGTH];

  uint16_t _tcpPort;

  WeakHandle<DHTPeerAnnounceStorage> _peerAnnounceStorage;

  WeakHandle<DHTTokenTracker> _tokenTracker;
public:
  DHTAnnouncePeerMessage(const SharedHandle<DHTNode>& localNode,
			 const SharedHandle<DHTNode>& remoteNode,
			 const unsigned char* infoHash,
			 uint16_t tcpPort,
			 const std::string& token,
			 const std::string& transactionID = "");

  virtual ~DHTAnnouncePeerMessage();

  virtual void doReceivedAction();

  virtual Dictionary* getArgument();
  
  virtual std::string getMessageType() const;

  virtual void validate() const;

  const unsigned char* getInfoHash() const
  {
    return _infoHash;
  }

  const std::string& getToken() const
  {
    return _token;
  }

  uint16_t getTCPPort() const
  {
    return _tcpPort;
  }

  void setPeerAnnounceStorage(const WeakHandle<DHTPeerAnnounceStorage>& storage);

  void setTokenTracker(const WeakHandle<DHTTokenTracker>& tokenTracker);
};

} // namespace aria2

#endif // _D_DHT_ANNOUNCE_PEER_MESSAGE_H_