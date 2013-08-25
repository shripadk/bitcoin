// Copyright (c) 2009-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include <hiredis/hiredis.h>

#include "hash.h"
#include "bitcoinrpc.h"
#include "main.h"

extern void TxToJSON(const CTransaction& tx, const uint256 hashBlock, json_spirit::Object& entry);
extern json_spirit::Object blockToJSON(const CBlock& block, const CBlockIndex* blockindex);

static redisContext *r_ctx = NULL;

void RedisNotifier_Init(const std::string& host, const int port)
{
  r_ctx = redisConnect(host.c_str(), port);
  if(r_ctx != NULL && r_ctx->err) {
    printf("Redis Connection Error: %s\n", r_ctx->errstr);
  }
}

void RedisNotifier_SendData(const std::string& data)
{
  void *reply = redisCommand(r_ctx, "LPUSH _bitcoin_notifications_ %s", data.c_str());
  freeReplyObject(reply);
}

void RedisNotifier_SendBlock(CBlockIndex* pblockindex)
{
  printf("%s\n", "SENDING BLOCK!");
  CBlock block;
  ReadBlockFromDisk(block, pblockindex);

  json_spirit::Object result = blockToJSON(block, pblockindex);
  std::string strTemp = json_spirit::write_string(json_spirit::Value(result), false);

  RedisNotifier_SendData(strTemp);
  printf("%s\n", "BLOCK SENT!");
}

void RedisNotifier_SendTx(const CTransaction tx)
{
  printf("%s\n", "SENDING TRANSACTION!");
  json_spirit::Object result;
  TxToJSON(tx, 0, result);

  std::string strTemp = json_spirit::write_string(json_spirit::Value(result), false);

  RedisNotifier_SendData(strTemp);
  printf("%s\n", "TRANSACTION SENT!");
}

void RedisNotifier_Cleanup()
{
  redisFree(r_ctx);
}