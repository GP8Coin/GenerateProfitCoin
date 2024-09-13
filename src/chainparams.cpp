// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "assert.h"

#include "chainparams.h"
#include "main.h"
#include "util.h"

#include <boost/assign/list_of.hpp>

using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

// Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress> &vSeedsOut, const SeedSpec6 *data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7*24*60*60;
    for (unsigned int i = 0; i < count; i++)
    {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

//
// Main network
//
class CMainParams : public CChainParams {
public:
    CMainParams() {
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0xbb;
        pchMessageStart[1] = 0xe1;
        pchMessageStart[2] = 0x7a;
        pchMessageStart[3] = 0xb1;
        vAlertPubKey = ParseHex("11efab1aa999e112756b659c91a56897eba2efaacb6a192acdbef7894465f81f85d131aadfef3be6145678454852a2d08c6314bba5ca3cbe5616262da3b1a6aea9");
        nDefaultPort = 20775; // peer
        nRPCPort = 20725; // rpc
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 18);
        bnProofOfStakeLimit = CBigNum(~uint256(0) >> 18);

        const char* pszTimestamp = "Whales Show Interest in Dogecoin (DOGE) and IntelMarkets As Solana (SOL) Sets New Record | Null Transaction PR - TheMerkle | September 12, 2024";
        std::vector<CTxIn> vin;
        vin.resize(1);
        vin[0].scriptSig = CScript() << 0 << CBigNum(42) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        std::vector<CTxOut> vout;
        vout.resize(1);
        vout[0].nValue = 1 * COIN;
        vout[0].SetEmpty();
        CTransaction txNew(1, 1726200000, vin, vout, 0);
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime    = 1726200000;
        genesis.nBits    = bnProofOfWorkLimit.GetCompact();
        genesis.nNonce   = 242521;

        /** Genesis Block MainNet */
        /*
Hashed MainNet Genesis Block Output
block.hashMerkleRoot == 70886a16c3ed7da116b56c5011d1f04d2d284b63bba2283e6fc55953bc104134
block.nTime = 1726200000
block.nNonce = 242521
block.GetHash = 00003f0ce4dd22098bb76129db94574947b405a88d265a401cd76c9a702d2fb6
        */

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x00003f0ce4dd22098bb76129db94574947b405a88d265a401cd76c9a702d2fb6"));
        assert(genesis.hashMerkleRoot == uint256("0x70886a16c3ed7da116b56c5011d1f04d2d284b63bba2283e6fc55953bc104134"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,38);// G
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,47);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,44);
        base58Prefixes[STEALTH_ADDRESS] = std::vector<unsigned char>(1,60);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();

        // vSeeds.push_back(CDNSSeedData("node0",  "000.000.000.000"));


        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        nPoolMaxTransactions = 9;
        strMNenginePoolDummyAddress = "Gg1uLByKq9DmxwRs7WDNUjLfX1GG5MmpmV";
        strDevOpsAddress = "GdwuN3ZwMNGHEAwr2kHsBzQ1EawHuuQykn";
        strPASfeeAddress = "GdwuN3ZwMNGHEAwr2kHsBzQ1EawHuuQykn";
        nEndPoWBlock = 0x7fffffff;
        nStartPoSBlock = 1;
    }

    virtual const CBlock& GenesisBlock() const { return genesis; }
    virtual Network NetworkID() const { return CChainParams::MAIN; }

    virtual const vector<CAddress>& FixedSeeds() const {
        return vFixedSeeds;
    }
protected:
    CBlock genesis;
    vector<CAddress> vFixedSeeds;
};
static CMainParams mainParams;


//
// Testnet
//
class CTestNetParams : public CMainParams {
public:
    CTestNetParams() {
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0xba;
        pchMessageStart[1] = 0xe2;
        pchMessageStart[2] = 0x7a;
        pchMessageStart[3] = 0x91;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 15);
        bnProofOfStakeLimit = CBigNum(~uint256(0) >> 15);
        vAlertPubKey = ParseHex("91efab1aa999e112756b659c91a56897eba2efaacb6a192acdbef7894465f81f85d131aadfef3be6145678454852a2d08c6314bba5ca3cbe5616262da3b1a6aea9");
        nDefaultPort = 20099; // 
        nRPCPort = 20198; //
        strDataDir = "testnet";

        // Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime  = 1726200020;
        genesis.nBits  = bnProofOfWorkLimit.GetCompact();
        genesis.nNonce = 1892;

        /** Genesis Block TestNet */
        /*
Hashed MainNet Genesis Block Output
block.hashMerkleRoot == 70886a16c3ed7da116b56c5011d1f04d2d284b63bba2283e6fc55953bc104134
block.nTime = 1726200020
block.nNonce = 1892
block.GetHash = 000188ac99b007ec13a3760b4c88c691c87e0a7fa856de7179af67b88fba9b64
        */

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x000188ac99b007ec13a3760b4c88c691c87e0a7fa856de7179af67b88fba9b64"));

        vFixedSeeds.clear();
        vSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,40);//
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,47);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,44);
        base58Prefixes[STEALTH_ADDRESS] = std::vector<unsigned char>(1,60);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        nEndPoWBlock = 0x7fffffff;
    }
    virtual Network NetworkID() const { return CChainParams::TESTNET; }
};
static CTestNetParams testNetParams;

//
// Regression test
//
class CRegTestParams : public CTestNetParams {
public:
    CRegTestParams() {
        pchMessageStart[0] = 0xae;
        pchMessageStart[1] = 0x21;
        pchMessageStart[2] = 0x77;
        pchMessageStart[3] = 0xb9;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 1);
        genesis.nTime = 1726200080;
        genesis.nBits  = bnProofOfWorkLimit.GetCompact();
        genesis.nNonce = 8;
        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 20009; //
        strDataDir = "regtest";

        /** Genesis Block RegNet */
        /*
Hashed MainNet Genesis Block Output
block.hashMerkleRoot == 70886a16c3ed7da116b56c5011d1f04d2d284b63bba2283e6fc55953bc104134
block.nTime = 1726200080
block.nNonce = 8
block.GetHash = 394c400095c505a4520450e529cd361fb8e69f3199027170e95c9fc9a6bc953c
        */

        assert(hashGenesisBlock == uint256("0x394c400095c505a4520450e529cd361fb8e69f3199027170e95c9fc9a6bc953c"));

        vSeeds.clear();  // Regtest mode doesn't have any DNS seeds.
    }

    virtual bool RequireRPCPassword() const { return false; }
    virtual Network NetworkID() const { return CChainParams::REGTEST; }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = &mainParams;

const CChainParams &Params() {
    return *pCurrentParams;
}

void SelectParams(CChainParams::Network network) {
    switch (network) {
        case CChainParams::MAIN:
            pCurrentParams = &mainParams;
            break;
        case CChainParams::TESTNET:
            pCurrentParams = &testNetParams;
            break;
        case CChainParams::REGTEST:
            pCurrentParams = &regTestParams;
            break;
        default:
            assert(false && "Unimplemented network");
            return;
    }
}

bool SelectParamsFromCommandLine() {
    bool fRegTest = GetBoolArg("-regtest", false);
    bool fTestNet = GetBoolArg("-testnet", false);

    if (fTestNet && fRegTest) {
        return false;
    }

    if (fRegTest) {
        SelectParams(CChainParams::REGTEST);
    } else if (fTestNet) {
        SelectParams(CChainParams::TESTNET);
    } else {
        SelectParams(CChainParams::MAIN);
    }
    return true;
}
