// Copyright (c) 2016-2023 The CryptoCoderz Team / Espers
// Copyright (c) 2018-2023 The CryptoCoderz Team / INSaNe project
// Copyright (c) 2018-2023 The Rubix project
// Copyright (c) 2022-2023 The GenerateProfitCoin project
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "blockparams.h"
#include "chainparams.h"
#include "checkpoints.h"
#include "db.h"
#include "init.h"
#include "kernel.h"
#include "net.h"
#include "txdb.h"
#include "velocity.h"
#include "main.h"
#include "mnengine.h"
#include "masternodeman.h"
#include "masternode-payments.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

using namespace std;
using namespace boost;

//////////////////////////////////////////////////////////////////////////////
//
// Standard Global Values
//

//
// Section defines global values for retarget logic
//

double VLF1 = 0;
double VLF2 = 0;
double VLF3 = 0;
double VLF4 = 0;
double VLF5 = 0;
double VLFtmp = 0;
double VRFsm1 = 1;
double VRFdw1 = 0.75;
double VRFdw2 = 0.5;
double VRFup1 = 1.25;
double VRFup2 = 1.5;
double VRFup3 = 2;
double TerminalAverage = 0;
double TerminalFactor = 10000;
double debugTerminalAverage = 0;
CBigNum newBN = 0;
CBigNum oldBN = 0;
int64_t VLrate1 = 0;
int64_t VLrate2 = 0;
int64_t VLrate3 = 0;
int64_t VLrate4 = 0;
int64_t VLrate5 = 0;
int64_t VLRtemp = 0;
int64_t DSrateNRM = BLOCK_SPACING;
int64_t DSrateMAX = BLOCK_SPACING_MAX;
int64_t FRrateDWN = DSrateNRM - 15;
int64_t FRrateFLR = DSrateNRM - 30;
int64_t FRrateCLNG = DSrateMAX;
int64_t difficultyfactor = 0;
int64_t AverageDivisor = 5;
int64_t scanheight = 6;
int64_t scanblocks = 1;
int64_t scantime_1 = 0;
int64_t scantime_2 = 0;
int64_t prevPoW = 0; // hybrid value
int64_t prevPoS = 0; // hybrid value
uint64_t blkTime = 0;
uint64_t cntTime = 0;
uint64_t prvTime = 0;
uint64_t difTime = 0;
uint64_t minuteRounds = 0;
uint64_t difCurve = 0;
uint64_t debugminuteRounds = 0;
uint64_t debugDifCurve = 0;
bool fDryRun;
bool fCRVreset;
const CBlockIndex* pindexPrev = 0;
const CBlockIndex* BlockVelocityType = 0;
CBigNum bnVelocity = 0;
CBigNum bnOld;
CBigNum bnNew;
std::string difType ("");
unsigned int retarget = DIFF_VRX; // Default with VRX


//////////////////////////////////////////////////////////////////////////////
//
// Debug section
//

//
// Debug log printing
//

void VRXswngdebug(bool fProofOfStake)
{
    // Print for debugging
    LogPrintf("Previously discovered %s block: %u: \n",difType.c_str(),prvTime);
    LogPrintf("Current block-time: %u: \n",cntTime);
    LogPrintf("Time since last %s block: %u: \n",difType.c_str(),difTime);
    // Handle updated versions as well as legacy
    if(nBestHeight > 200) {
        debugminuteRounds = minuteRounds;
        debugTerminalAverage = TerminalAverage;
        debugDifCurve = difCurve;
        LogPrintf("VRX_Threadcurve - Previous difficulty found: %d \n", VRX_GetPrevDiff(fProofOfStake));
        while(difTime > (debugminuteRounds * 60)) {
            if(VRX_GetPrevDiff(fProofOfStake) < 1) {
                // Skip
                LogPrintf("VRX_Threadcurve - Difficulty too low for curve, skipping \n");
                return;
            }
            debugTerminalAverage /= debugDifCurve;
            LogPrintf("diffTime%s is greater than %u Minutes: %u \n",difType.c_str(),debugminuteRounds,cntTime);
            LogPrintf("Difficulty will be multiplied by: %d \n",debugTerminalAverage);
            // Break loop after 20 minutes, otherwise time threshold will auto-break loop
            if (debugminuteRounds > (5 + 15)){
                break;
            }
            debugDifCurve ++;
            debugminuteRounds ++;
        }
    } else {
        if(difTime > (minuteRounds+0) * 60 * 60) {LogPrintf("diffTime%s is greater than 1 Hours: %u \n",difType.c_str(),cntTime);}
        if(difTime > (minuteRounds+1) * 60 * 60) {LogPrintf("diffTime%s is greater than 2 Hours: %u \n",difType.c_str(),cntTime);}
        if(difTime > (minuteRounds+2) * 60 * 60) {LogPrintf("diffTime%s is greater than 3 Hours: %u \n",difType.c_str(),cntTime);}
        if(difTime > (minuteRounds+3) * 60 * 60) {LogPrintf("diffTime%s is greater than 4 Hours: %u \n",difType.c_str(),cntTime);}
    }

    return;
}

void VRXdebug()
{
    // Print for debugging
    LogPrintf("Terminal-Velocity 1st spacing: %u: \n",VLrate1);
    LogPrintf("Terminal-Velocity 2nd spacing: %u: \n",VLrate2);
    LogPrintf("Terminal-Velocity 3rd spacing: %u: \n",VLrate3);
    LogPrintf("Terminal-Velocity 4th spacing: %u: \n",VLrate4);
    LogPrintf("Terminal-Velocity 5th spacing: %u: \n",VLrate5);
    LogPrintf("Desired normal spacing: %u: \n",DSrateNRM);
    LogPrintf("Desired maximum spacing: %u: \n",DSrateMAX);
    LogPrintf("Terminal-Velocity 1st multiplier set to: %f: \n",VLF1);
    LogPrintf("Terminal-Velocity 2nd multiplier set to: %f: \n",VLF2);
    LogPrintf("Terminal-Velocity 3rd multiplier set to: %f: \n",VLF3);
    LogPrintf("Terminal-Velocity 4th multiplier set to: %f: \n",VLF4);
    LogPrintf("Terminal-Velocity 5th multiplier set to: %f: \n",VLF5);
    LogPrintf("Terminal-Velocity averaged a final multiplier of: %f: \n",TerminalAverage);
    LogPrintf("Prior Terminal-Velocity: %u\n", oldBN);
    LogPrintf("New Terminal-Velocity:  %u\n", newBN);
    return;
}

void GNTdebug()
{
    // Print for debugging
    // Retarget ignoring invalid selection
    if (retarget != DIFF_VRX)
    {
        // debug info for testing
        LogPrintf("GetNextTargetRequired() : Invalid retarget selection, using default \n");
        return;
    }

    // Retarget using Terminal-Velocity
    // debug info for testing
    LogPrintf("Terminal-Velocity retarget selected \n");
    LogPrintf("Espers retargetted using: Terminal-Velocity difficulty curve \n");
    return;
}


//////////////////////////////////////////////////////////////////////////////
//
// Difficulty retarget (current section)
//

//
// This is VRX (v3.7) revised implementation
//
// Terminal-Velocity-RateX, v10-Beta-R9, written by Jonathan Dan Zaretsky - cryptocoderz@gmail.com
void VRX_BaseEngine(const CBlockIndex* pindexLast, bool fProofOfStake)
{
       // Set base values
       VLF1 = 0;
       VLF2 = 0;
       VLF3 = 0;
       VLF4 = 0;
       VLF5 = 0;
       VLFtmp = 0;
       TerminalAverage = 0;
       TerminalFactor = 10000;
       VLrate1 = 0;
       VLrate2 = 0;
       VLrate3 = 0;
       VLrate4 = 0;
       VLrate5 = 0;
       VLRtemp = 0;
       difficultyfactor = 0;
       scanblocks = 1;
       scantime_1 = 0;
       scantime_2 = pindexLast->GetBlockTime();
       prevPoW = 0; // hybrid value
       prevPoS = 0; // hybrid value
       // Set prev blocks...
       pindexPrev = pindexLast;
       // ...and deduce spacing
       while(scanblocks < scanheight)
       {
           scantime_1 = scantime_2;
           pindexPrev = pindexPrev->pprev;
           scantime_2 = pindexPrev->GetBlockTime();
           // Set standard values
           if(scanblocks > 0){
               if     (scanblocks < scanheight-4){ VLrate1 = (scantime_1 - scantime_2); VLRtemp = VLrate1; }
               else if(scanblocks < scanheight-3){ VLrate2 = (scantime_1 - scantime_2); VLRtemp = VLrate2; }
               else if(scanblocks < scanheight-2){ VLrate3 = (scantime_1 - scantime_2); VLRtemp = VLrate3; }
               else if(scanblocks < scanheight-1){ VLrate4 = (scantime_1 - scantime_2); VLRtemp = VLrate4; }
               else if(scanblocks < scanheight-0){ VLrate5 = (scantime_1 - scantime_2); VLRtemp = VLrate5; }
           }
           // Round factoring
           if(VLRtemp >= DSrateNRM){ VLFtmp = VRFsm1;
               if(VLRtemp > DSrateMAX){ VLFtmp = VRFdw1;
                   if(VLRtemp > FRrateCLNG){ VLFtmp = VRFdw2; }
               }
           }
           else if(VLRtemp < DSrateNRM){ VLFtmp = VRFup1;
               if(VLRtemp < FRrateDWN){ VLFtmp = VRFup2;
                   if(VLRtemp < FRrateFLR){ VLFtmp = VRFup3; }
               }
           }
           // Record factoring
           if      (scanblocks < scanheight-4) VLF1 = VLFtmp;
           else if (scanblocks < scanheight-3) VLF2 = VLFtmp;
           else if (scanblocks < scanheight-2) VLF3 = VLFtmp;
           else if (scanblocks < scanheight-1) VLF4 = VLFtmp;
           else if (scanblocks < scanheight-0) VLF5 = VLFtmp;
           // Log hybrid block type
           //
           // v1.0
           if      (fProofOfStake) prevPoS ++;
           else if(!fProofOfStake) prevPoW ++;
           // v1.1
           if(pindexPrev->IsProofOfStake()) { prevPoS ++; }
           else if(pindexPrev->IsProofOfWork()) { prevPoW ++; }

           // move up per scan round
           scanblocks ++;
       }
       // Final mathematics
       TerminalAverage = (VLF1 + VLF2 + VLF3 + VLF4 + VLF5) / AverageDivisor;
       return;
}

void VRX_Simulate_Retarget()
{
    // Perform retarget simulation
    TerminalFactor *= TerminalAverage;
    difficultyfactor = TerminalFactor;
    bnOld.SetCompact(BlockVelocityType->nBits);
    bnNew = bnOld / difficultyfactor;
    bnNew *= 10000;
    // Reset TerminalFactor for actual retarget
    TerminalFactor = 10000;
    return;
}

double VRX_GetPrevDiff(bool fPoS)
{
    const CBlockIndex* blockindex;

    // Floating point number that is a multiple of the minimum difficulty,
    // minimum difficulty = 1.0.
    if (pindexBest == NULL) {
        return 1.0;
    } else {
        blockindex = GetLastBlockIndex(pindexBest, fPoS);
    }

    int nShift = (blockindex->nBits >> 24) & 0xff;

    double dDiff =
        (double)0x0000ffff / (double)(blockindex->nBits & 0x00ffffff);

    while (nShift < 29)
    {
        dDiff *= 256.0;
        nShift++;
    }
    while (nShift > 29)
    {
        dDiff /= 256.0;
        nShift--;
    }

    return dDiff;
}

void VRX_ThreadCurve(const CBlockIndex* pindexLast, bool fProofOfStake)
{
    // Run VRX engine
    VRX_BaseEngine(pindexLast, fProofOfStake);

    //
    // Skew for less selected block type
    //

    // Version 1.0
    //
    if(prevPoW < prevPoS && !fProofOfStake){if((prevPoS-prevPoW) > 3) TerminalAverage /= 3;}
    else if(prevPoW > prevPoS && fProofOfStake){if((prevPoW-prevPoS) > 3) TerminalAverage /= 3;}
    if(TerminalAverage < 0.5) TerminalAverage = 0.5; // limit skew to halving

    // Version 1.1 curve-patch
    //
    if(1 == 1)
    {
        // Define time values
        blkTime = pindexLast->GetBlockTime();
        int64_t prvrawTime = pindexLast->pprev->GetBlockTime();
        cntTime = BlockVelocityType->GetBlockTime();
        prvTime = BlockVelocityType->pprev->GetBlockTime();
        difTime = cntTime - prvTime;
        minuteRounds = 5;
        difCurve = 2;
        fCRVreset = false;

        // Debug print toggle
        if(fProofOfStake) {
            difType = "PoS";
        } else {
            difType = "PoW";
        }
        if(fDebug) VRXswngdebug(fProofOfStake);

        // Version 1.3 Extended Curve Run Upgrade
        if(pindexLast->nHeight > 110) {
            // Set unbiased comparison
            difTime = blkTime - prvrawTime;
            // Run Curve
            while(difTime > (minuteRounds * 60)) {
                // Skip Extended Curve Run if diff is too low
                if(VRX_GetPrevDiff(fProofOfStake) < 1) {
                    // Skip
                    break;
                }
                // Break loop after 8 minutes, otherwise time threshold will auto-break loop
                if(minuteRounds > 8){
                    fCRVreset = true;
                    break;
                }
                // Drop difficulty per round
                TerminalAverage /= difCurve;
                // Simulate retarget for sanity
                VRX_Simulate_Retarget();
                // Increase Curve per round
                difCurve ++;
                // Move up a minute per round
                minuteRounds += 1;
            }
        } else {// Version 1.1 Standard Curve Run
            if(difTime > (minuteRounds+0) * 60 * 60) { TerminalAverage /= difCurve; }
            if(difTime > (minuteRounds+1) * 60 * 60) { TerminalAverage /= difCurve; }
            if(difTime > (minuteRounds+2) * 60 * 60) { TerminalAverage /= difCurve; }
            if(difTime > (minuteRounds+3) * 60 * 60) { TerminalAverage /= difCurve; }
        }
    }
    return;
}

void VRX_Dry_Run(const CBlockIndex* pindexLast)
{
    // Check for blocks to index | Allowing for initial chain start
    if (pindexLast->nHeight < scanheight+110) {
        fDryRun = true;
        return; // can't index prevblock
    }

    // Test Fork
    if (nLiveForkToggle != 0) {
        // Do nothing
    }// TODO setup next testing fork

    // Standard, non-Dry Run
    fDryRun = false;
    return;
}

unsigned int VRX_Retarget(const CBlockIndex* pindexLast, bool fProofOfStake)
{
    // Set base values
    bnVelocity = fProofOfStake ? Params().ProofOfStakeLimit() : Params().ProofOfWorkLimit();

    // Differentiate PoW/PoS prev block
    BlockVelocityType = GetLastBlockIndex(pindexLast, fProofOfStake);

    // Check for a dry run
    VRX_Dry_Run(pindexLast);
    if(fDryRun) { return bnVelocity.GetCompact(); }

    // Run VRX threadcurve
    VRX_ThreadCurve(pindexLast, fProofOfStake);
    if (fCRVreset) { return bnVelocity.GetCompact(); }

    // Retarget using simulation
    VRX_Simulate_Retarget();

    // Limit
    if (bnNew > bnVelocity) { bnNew = bnVelocity; }

    // Final log
    oldBN = bnOld.GetCompact();
    newBN = bnNew.GetCompact();

    // Debug print toggle
    if(fDebug) VRXdebug();

    // Return difficulty
    return bnNew.GetCompact();
}

//////////////////////////////////////////////////////////////////////////////
//
// Difficulty retarget (function)
//

unsigned int GetNextTargetRequired(const CBlockIndex* pindexLast, bool fProofOfStake)
{
    // Default with VRX
    unsigned int retarget = DIFF_VRX;

    // Check selection
    if (retarget != DIFF_VRX)
    {
        // debug info for testing
        if(fDebug) GNTdebug();
        return VRX_Retarget(pindexLast, fProofOfStake);
    }

    // Retarget using Terminal-Velocity
    // debug info for testing
    if(fDebug) GNTdebug();
    return VRX_Retarget(pindexLast, fProofOfStake);
}

//////////////////////////////////////////////////////////////////////////////
//
// Coin base subsidy
//

//
// Masternode Tier 2 Payout Toggle
//
bool fMNtier2()
{
    return false;
}

//
// PoW coin base reward
//
int64_t GetProofOfWorkReward(int nHeight, int64_t nFees)
{
    int64_t nSubsidy = 20 * COIN;

    if(nHeight > nReservePhaseStart) {
        if(pindexBest->nMoneySupply < (nBlockRewardReserve * 100)) {
            nSubsidy = nBlockRewardReserve;
        }
    }

    // hardCap v2.1
    else if(pindexBest->nMoneySupply > MAX_SINGLE_TX)
    {
        LogPrint("MINEOUT", "GetProofOfWorkReward(): create=%s nFees=%d\n", FormatMoney(nFees), nFees);
        return nFees;
    }

    // Halving -- OFF
    // nSubsidy >>= (nHeight / 500000); // Halves every 500,000 blocks

    LogPrint("creation", "GetProofOfWorkReward() : create=%s nSubsidy=%d\n", FormatMoney(nSubsidy), nSubsidy);
    return nSubsidy + nFees;
}

//
// PoS coin base reward
//
int64_t GetProofOfStakeReward(const CBlockIndex* pindexPrev, int64_t nCoinAge, int64_t nFees)
{
    int64_t nSubsidy = nBlockStandardReward; // PoS Staking - pindexPrev is info from the last block, and -> means to get specific info from that block. Getblocktime is the epoch time of that block.

    if(pindexPrev->nHeight+1 > nReservePhaseStart) { // If, all 100 blocks of the premine are not done, then next blocks have premine value
        if(pindexBest->nMoneySupply < (nBlockRewardReserve * 100)) {
            nSubsidy = nBlockRewardReserve;
        }
    }

    // hardCap v2.1
    else if(pindexBest->nMoneySupply > MAX_SINGLE_TX)
    {
        LogPrint("MINEOUT", "GetProofOfStakeReward(): create=%s nFees=%d\n", FormatMoney(nFees), nFees);
        return nFees;
    }

    LogPrint("creation", "GetProofOfStakeReward(): create=%s nCoinAge=%d\n", FormatMoney(nSubsidy), nCoinAge);
    return nSubsidy + nFees;
}

//
// Masternode coin base reward
//
int64_t GetMasternodePayment(int nHeight, int64_t blockValue)
{
    // Define values
    int64_t  ret2 = 0;
    if(nHeight > 0) {
        ret2 = (17 * COIN);
    }
    return ret2;
}

//
// DevOps coin base reward
//
int64_t GetDevOpsPayment(int nHeight, int64_t blockValue)
{
    int64_t ret2 = 0;
    if(nHeight > 0) {
        ret2 = (3 * COIN);
    }
    return ret2;
}
