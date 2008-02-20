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
#ifndef _D_DHT_PEER_ANNOUNCE_ENTRY_H_
#define _D_DHT_PEER_ANNOUNCE_ENTRY_H_

#include "common.h"
#include "SharedHandle.h"
#include "DHTConstants.h"
#include "PeerAddrEntry.h"
#include "TimeA2.h"
#include <deque>

namespace aria2 {

class Peer;
class BtContext;

class DHTPeerAnnounceEntry {
private:
  unsigned char _infoHash[DHT_ID_LENGTH];

  std::deque<PeerAddrEntry> _peerAddrEntries;

  SharedHandle<BtContext> _btCtx;

  Time _lastUpdated;
public:
  DHTPeerAnnounceEntry(const unsigned char* infoHash);

  ~DHTPeerAnnounceEntry();

  // add peer addr entry.
  // if it already exists, update "Last Updated" property.
  void addPeerAddrEntry(const PeerAddrEntry& entry);

  void setBtContext(const SharedHandle<BtContext>& btCtx);

  size_t countPeerAddrEntry() const;

  const std::deque<PeerAddrEntry>& getPeerAddrEntries() const;

  void removeStalePeerAddrEntry(time_t timeout);
  
  bool empty() const;

  const Time& getLastUpdated() const
  {
    return _lastUpdated;
  }

  void notifyUpdate();

  const unsigned char* getInfoHash() const
  {
    return _infoHash;
  }

  std::deque<SharedHandle<Peer> > getPeers() const;

};

} // namespace aria2

#endif // _D_DHT_PEER_ANNOUNCE_ENTRY_H_