// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (0,        Params().HashGenesisBlock() )
        (1,        uint256("0x00002111f57916be5f4d972029332c90af981fe159c5df8675fc5e32e231f79d"))
        (10,       uint256("0x00002de6cf6f1d5939c87d643daaa16a21ee75bbad3017e621d499dc99dd159c"))
        (100,      uint256("0x00002cf0707e9911050b1b898b1d171f74ba122a48baa612c0f83e965fedaba2"))
        (101,      uint256("0x000016ecbe90d05597540020da8947d4c6f58e52a3909ff943ffc9453f7b03a4"))
        //(500,      uint256("0xf44e0c14e34132145dff5ae5a8c5e93172d7ac89b2cff88497c89f1c2fc183a0"))
        //(562,      uint256("0x2b53c1e333354b9576c0bbe97884c1488fa21d3a2a5847123a96ac6b1e8fd4b9"))
        //(725,      uint256("0xe4b29993f9e5151fa51245318a124326e8b86593fcb1640d6205669bc3c3462d"))
        //(757,      uint256("0xb03f60aef1b721c068423a0971c8e65ed6ab5b6532592914bf0adde478bf6329"))
    ;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        // Taking into account our 120 block depth + reorganize depth
        if(pindexBest->nHeight < 500) {
            if(pindexBest->nHeight < 1) {
                return pindex;
            } else {
                return pindex->pprev;
            }
        }
        // Search backward for a block within max span and maturity window
        // Taking into account our 120 block depth + reorganize depth
        while (pindex->pprev && pindex->nHeight + (BLOCK_TEMP_CHECKPOINT_DEPTH + 500) > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();
        if (nHeight <= pindexSync->nHeight){
            return false;
        }
        return true;
    }
}
