#define BOOST_TEST_MODULE DNSTests
#include <boost/test/unit_test.hpp>
#include <fc/io/raw.hpp>
#include <fc/log/logger.hpp>
#include <fc/filesystem.hpp>
#include <iostream>

#include <bts/blockchain/config.hpp>
#include <bts/blockchain/chain_database.hpp>
#include <bts/wallet/wallet.hpp>

#include <bts/dns/dns_wallet.hpp>


using namespace bts::wallet;
using namespace bts::blockchain;
using namespace bts::dns;

trx_block generate_genesis_block( const std::vector<address>& addr )
{
    trx_block genesis;
    genesis.version           = 0;
    genesis.block_num         = 0;
    genesis.timestamp         = fc::time_point::now();
    genesis.next_fee          = block_header::min_fee();
    genesis.total_shares      = 0;
    genesis.votes_cast        = 0;
    genesis.noncea            = 0;
    genesis.nonceb            = 0;
    genesis.next_difficulty   = 0;

    signed_transaction trx;
    for( uint32_t i = 0; i < addr.size(); ++i )
    {
        uint64_t amnt = rand()%1000 * BTS_BLOCKCHAIN_SHARE;
        trx.outputs.push_back( trx_output( claim_by_signature_output( addr[i] ), asset( amnt ) ) );
        genesis.total_shares += amnt;
    }
    genesis.available_votes   = genesis.total_shares;
    genesis.trxs.push_back( trx );
    genesis.trx_mroot = genesis.calculate_merkle_root(signed_transactions());

    return genesis;
}


/* 
 */
BOOST_AUTO_TEST_CASE ( templ )
{
    try {

    }
    catch (const fc::exception& e)
    {

        throw;
    }
}

/* You should be able to start a new auction for a name that the
 * network has never seen before
 */
BOOST_AUTO_TEST_CASE( new_auction_for_new_name )
{
    try {
        fc::temp_directory     dir;
        dns_wallet             wlt;
        std::vector<address>   addrs;
        dns_db                 dns_db;
        fc::ecc::private_key   auth;

        auto sim_validator = std::make_shared<sim_pow_validator>( fc::time_point::now() );
        auth = fc::ecc::private_key::generate();

        wlt.create( dir.path() / "wallet.dat", "password", "password", true );

        addrs.reserve(100);
        for( uint32_t i = 0; i < 10; ++i )
        {
            addrs.push_back( wlt.new_recv_address() );
        }

        auto addr = wlt.new_recv_address();
        addrs.push_back( addr );

        dns_db.set_signing_authority( auth.get_public_key() );
        dns_db.set_pow_validator( sim_validator );
        dns_db.open( dir.path() / "dns_db", true);
        dns_db.push_block( generate_genesis_block( addrs ) );
        auto head_id = dns_db.head_block_id();

        dns_db.set_block_signature( head_id, 
                auth.sign_compact( fc::sha256::hash((char*)&head_id, sizeof(head_id)) ));

        wlt.scan_chain( dns_db );
        wlt.dump();
        
        sim_validator->skip_time( fc::seconds(60 * 5) );

        std::vector<signed_transaction> txs;

        for (auto i = 0; i < 1; ++i )
        {
            auto transfer_tx = wlt.transfer(asset(uint64_t(1000000)), addrs[rand()%addrs.size()]);
            txs.push_back( transfer_tx );
        }

        auto buy_tx = wlt.buy_domain( "TESTNAME", asset(uint64_t(1)), dns_db );
        wlog( "buy_trx: ${trx} ", ("trx",buy_tx) );

        txs.push_back( buy_tx );
        
        auto next_block = wlt.generate_next_block( dns_db, txs );
        dns_db.push_block( next_block );

        wlt.scan_chain( dns_db );

    }
    catch (const fc::exception& e)
    {
        elog( "${e}", ("e",e.to_detail_string()) );
        throw;
    }

}


/* You should be able to start a new auction for an expired name
 */
BOOST_AUTO_TEST_CASE( new_auction_for_expired_name )
{

}

/* You should not be able to start a new auction on a domain that exists
 * and is not expired
 */
BOOST_AUTO_TEST_CASE( new_auction_for_unexpired_name_fail )
{

}

/* You should not be able to start an auction for an invalid name (length)
 */
BOOST_AUTO_TEST_CASE( new_auction_name_length_fail )
{

}

/* You should be able to bid on a domain that is in an auction. The previous
 * owner should get the right amount of funds and the fee should be big enough
 */
BOOST_AUTO_TEST_CASE( bid_on_auction )
{

}

/* Your bid should fail if the domain is not in an auction
 * TODO does this duplicate "new_auction_for_unexpired_name_fail"? Do the txs look
 * different?
 */
BOOST_AUTO_TEST_CASE( bid_fail_not_in_auction )
{

}

/* Your bid should fail if the fee is not sufficient
 */
BOOST_AUTO_TEST_CASE( bid_fail_insufficient_fee )
{

}

/* Your bid should fail if you don't pay the previous owner enough
 */
BOOST_AUTO_TEST_CASE( bid_fail_prev_owner_payment )
{

}

/* You should be able to update a record if you still own the domain
 */
BOOST_AUTO_TEST_CASE( update_record )
{

}

/* You should not be able to update a record if you don't own the domain (wrong sig)
 */
BOOST_AUTO_TEST_CASE( update_record_sig_fail )
{

}

/* You should not be able to update a record if you don't own the domain (expired)
 */
BOOST_AUTO_TEST_CASE( update_record_expire_fail )
{

}

/* You should not be able to update a record with an invalid value (length)
 */
BOOST_AUTO_TEST_CASE( update_record_val_length_fail )
{

}

/* You should be able to sell a domain if you own it
 */
BOOST_AUTO_TEST_CASE( sell_domain )
{

}

/* You should not be able to sell a domain if you don't own it (sig)
 */
BOOST_AUTO_TEST_CASE( sell_domain_sig_fail )
{

}

/* You should not be able to sell a domain if you don't own it (expired)
 */
BOOST_AUTO_TEST_CASE( sell_domain_expire_fail )
{

}
