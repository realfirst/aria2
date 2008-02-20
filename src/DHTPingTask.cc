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
#include "DHTPingTask.h"
#include "DHTMessageCallbackImpl.h"
#include "DHTMessage.h"
#include "DHTMessageFactory.h"
#include "DHTMessageDispatcher.h"
#include "DHTNode.h"
#include "DHTConstants.h"

namespace aria2 {

DHTPingTask::DHTPingTask(const SharedHandle<DHTNode>& remoteNode, size_t numMaxRetry):
  _remoteNode(remoteNode),
  _numMaxRetry(numMaxRetry),
  _numRetry(0),
  _pingSuccessful(false),
  _timeout(DHT_MESSAGE_TIMEOUT)
{}

DHTPingTask::~DHTPingTask() {}

void DHTPingTask::startup()
{
  SharedHandle<DHTMessage> m = _factory->createPingMessage(_remoteNode);
  _dispatcher->addMessageToQueue(m, _timeout, new DHTMessageCallbackImpl(this));
}

void DHTPingTask::onReceived(const SharedHandle<DHTMessage>& message)
{
  _pingSuccessful = true;
  _finished = true;
}

void DHTPingTask::onTimeout(const SharedHandle<DHTNode>& node)
{
  ++_numRetry;
  if(_numRetry >= _numMaxRetry) {
    _pingSuccessful = false;
    _finished = true;
  } else {
    SharedHandle<DHTMessage> m = _factory->createPingMessage(_remoteNode);
    _dispatcher->addMessageToQueue(m, _timeout, new DHTMessageCallbackImpl(this));
  }
}

bool DHTPingTask::isPingSuccessful() const
{
  return _pingSuccessful;
}

} // namespace aria2