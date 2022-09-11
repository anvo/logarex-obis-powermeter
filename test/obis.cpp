/*
 * Copyright (c) 2022 anvo
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */
#include <gtest/gtest.h>

#include <obis.h>

using namespace obis;

TEST(OBIS, ExtractValueFromASCIILine) {
  data data = parse_line("1-0:1.8.0*255(002245.8545*kWh)");
  EXPECT_EQ(data.code, "1.8.0");
  EXPECT_EQ(data.value, "002245.8545*kWh");

  data = parse_line("1-0:0.2.0*255(ver.03,9067,20170504)");
  EXPECT_EQ(data.code, "0.2.0");
  EXPECT_EQ(data.value, "ver.03,9067,20170504");  
}

TEST(OBIS, MalformedASCIILine) {
  data data = parse_line("nocontent");
  EXPECT_EQ(data.code, "");
  EXPECT_EQ(data.value, "");
}

TEST(OBIS, ParseMultiASCII) {
  std::vector<data> data = parse_lines("1-0:1.8.0*255(002296.1275*kWh)\n1-0:0.2.0*255(ver.03,9067,20170504)");
  EXPECT_EQ(data.size(), 2);
  EXPECT_EQ(data.at(0).code, "1.8.0");
  EXPECT_EQ(data.at(0).value, "002296.1275*kWh");
  EXPECT_EQ(data.at(1).code, "0.2.0");
  EXPECT_EQ(data.at(1).value, "ver.03,9067,20170504");    
}

TEST(OBIS, IgnoreNonValuesData) {
  std::vector<data> data = parse_lines("LOG5LK13BE803719\n\n1-0:1.8.0*255(003296.8575*kWh)\n!\n");
  EXPECT_EQ(data.size(), 1);
  EXPECT_EQ(data.at(0).code, "1.8.0");
  EXPECT_EQ(data.at(0).value, "003296.8575*kWh");
}

TEST(OBIS, EmptyData) {
  std::vector<data> data = parse_lines("");
  EXPECT_EQ(data.size(), 0);
}

TEST(OBIS, KWhValue) {
  EXPECT_EQ(parse_kwh("002596.8575*kWh"), 2596.8575);
}

TEST(OBIS, KWhValueError) {
  EXPECT_EQ(parse_kwh("LOG5LK13BE803919"), 0);
}