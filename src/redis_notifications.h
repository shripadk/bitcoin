// Copyright (c) 2009-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef _REDIS_NOTIFICATIONS
#define _REDIS_NOTIFICATIONS 1

#include "main.h"

void RedisNotifier_Init(const std::string&, const int);
void RedisNotifier_Cleanup();
void RedisNotifier_SendData(const std::string&);
void RedisNotifier_SendBlock(CBlockIndex*);
void RedisNotifier_SendTx(const CTransaction);

#endif