/*
 * Test on class HwIdentifier
 *
 *  Created on: Dec 26, 2019
 *      Author: devel
 */

#define BOOST_TEST_MODULE test_hw_identifier

#include <boost/test/unit_test.hpp>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <cstring>
#include <boost/filesystem.hpp>
#include <licensecc_properties.h>
#include <licensecc_properties_test.h>

#include <licensecc/licensecc.h>
#include "../../../src/library/hw_identifier/hw_identifier.hpp"

namespace license {
namespace test {
using namespace std;
using namespace license::hw_identifier;

/**
 * Test get and set and compare hardware identifier data
 */
BOOST_AUTO_TEST_CASE(set_and_compare_data) {
	array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> data = {0xFF, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42};
	HwIdentifier pc_id;
	pc_id.set_data(data);
	data[0] = data[0] & 0x1f;
	BOOST_CHECK_MESSAGE(pc_id.data_match(data), "Data match");
}
/**
 * Test get and set and compare hardware identifier data
 */
BOOST_AUTO_TEST_CASE(compare_wrong_data) {
	array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> data = {0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42};
	HwIdentifier pc_id;
	pc_id.set_data(data);
	data[4] = 0;
	BOOST_CHECK_MESSAGE(!pc_id.data_match(data), "Data shouldn't match");
}

/**
 * Print a hardware identifier and read it from the same string, check the data matches
 */
BOOST_AUTO_TEST_CASE(print_and_read) {
	array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> data = {0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42};
	HwIdentifier pc_id;
	pc_id.set_data(data);
	pc_id.set_identification_strategy(LCC_API_HW_IDENTIFICATION_STRATEGY::STRATEGY_ETHERNET);
	string pc_id_str = pc_id.print();
	cout << pc_id_str << endl;
	const HwIdentifier id2(pc_id_str);
	BOOST_CHECK_MESSAGE(id2.get_identification_strategy() == LCC_API_HW_IDENTIFICATION_STRATEGY::STRATEGY_ETHERNET,
						"Strategy decoded correctly");
	BOOST_CHECK_MESSAGE(id2.data_match(data), "Data deserialized correctly");
}

}  // namespace test
}  // namespace license
