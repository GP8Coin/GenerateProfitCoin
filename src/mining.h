// Copyright (c) 2016-2023 The CryptoCoderz Team / Espers
// Copyright (c) 2018-2023 The CryptoCoderz Team / INSaNe project
// Copyright (c) 2018-2023 The Rubix project
// Copyright (c) 2022-2023 The GenerateProfitCoin project
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_MINING_H
#define BITCOIN_MINING_H

#include "bignum.h"
/** Minimum nCoinAge required to stake PoS */
static const unsigned int nStakeMinAge = 4 * 60; // 1 Minute
/** Time to elapse before new modifier is computed */
static const unsigned int nModifierInterval = 2 * 60;
/** Genesis block subsidy */
static const int64_t nGenesisBlockReward = 1 * COIN;
/** Reserve block subsidy */
static const int64_t nBlockRewardReserve = 5000000 * COIN; // premine 500,000,000 GP8
/** Standard block subsidy */
static const int64_t nBlockStandardReward = (25 * COIN); // 25 GP8
/** Block spacing preferred */
static const int64_t BLOCK_SPACING = (1 * 60); // One minute (1 Min)
/** Block spacing minimum */
static const int64_t BLOCK_SPACING_MIN = (1 * 30); // Thirty seconds (30 Sec)
/** Block spacing maximum */
static const int64_t BLOCK_SPACING_MAX = (2 * 60); // Two minutes (2 Min)
/** Desired block times/spacing */
static const int64_t GetTargetSpacing = BLOCK_SPACING;
/** MNengine collateral */
static const int64_t MNengine_COLLATERAL = (1 * COIN);
/** MNengine pool values */
static const int64_t MNengine_POOL_MAX = (999 * COIN);
/** MasterNode required collateral */
inline int64_t MasternodeCollateral(int nHeight) { return 2000000; } // ON (2,000,000 GP8)
/** PubkeyAliasService required fee */
inline int64_t PubkeyaliasserviceFEE(int nHeight) { return 75; } // ON (75 GP8)
/** Coinbase transaction outputs can only be staked after this number of new blocks (network rule) */
static const int nStakeMinConfirmations = 10;
/** Coinbase transaction outputs can only be spent after this number of new blocks (network rule) */
static const int nTransactionMaturity = 6; // 6-TXs
/** Coinbase generated outputs can only be spent after this number of new blocks (network rule) */
static const int nCoinbaseMaturity = 10; // 10-Mined


#endif // BITCOIN_MINING_H
