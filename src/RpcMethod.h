/* <!-- copyright */
/*
 * aria2 - The high speed download utility
 *
 * Copyright (C) 2009 Tatsuhiro Tsujikawa
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
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
#ifndef D_RPC_METHOD_H
#define D_RPC_METHOD_H

#include "common.h"

#include <string>

#include "SharedHandle.h"
#include "ValueBase.h"

namespace aria2 {

class DownloadEngine;
class OptionParser;
class Option;
class Exception;

namespace rpc {

struct RpcRequest;
struct RpcResponse;

// This class offers abstract implementation of processing RPC
// request. You have to inherit this class and implement process()
// method to add new RPC API.
//
// There is RpcMethodFactory class which instantiates RpcMethod
// subclass. If you add new RpcMethod subclass, don't forget to add it
// to RpcMethodFactory.
class RpcMethod {
private:
  SharedHandle<OptionParser> optionParser_;
  bool jsonRpc_;
protected:
  // Subclass must implement this function to fulfil RpcRequest req.
  // The return value of this method is used as a return value of RPC
  // request.
  virtual SharedHandle<ValueBase> process
  (const RpcRequest& req, DownloadEngine* e) = 0;

  void gatherRequestOption
  (const SharedHandle<Option>& option, const Dict* optionsDict);

  void gatherChangeableOption
  (const SharedHandle<Option>& option, const Dict* optionDict);

  // Copy options which is changeable in RPC changeOption command to
  // dest.
  void applyChangeableOption(Option* dest, Option* src) const;

  void gatherChangeableGlobalOption(const SharedHandle<Option>& option,
                                    const Dict* optionDict);

  // Copy options which is changeable in RPC changeGlobalOption
  // command to dest.
  void applyChangeableGlobalOption(Option* dest, Option* src) const;

  SharedHandle<ValueBase> createErrorResponse(const Exception& e);

  const SharedHandle<OptionParser>& getOptionParser() const
  {
    return optionParser_;
  }
public:
  RpcMethod();

  virtual ~RpcMethod();

  // Do work to fulfill RpcRequest req and returns its result as
  // RpcResponse. This method delegates to process() method.
  RpcResponse execute(const RpcRequest& req, DownloadEngine* e);
  // Set whether JSON-RPC style parameter handling.
  void setJsonRpc(bool f)
  {
    jsonRpc_ = f;
  }
  bool getJsonRpc() const
  {
    return jsonRpc_;
  }
};

} // namespace rpc

} // namespace aria2

#endif // D_RPC_METHOD_H
