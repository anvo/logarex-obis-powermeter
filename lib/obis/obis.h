/*
 * Copyright (c) 2022 anvo
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */
#pragma once

#include <vector>
#include <string>

namespace obis {

    struct data
    {
        std::string code;
        std::string value;
    };

    // Parse a single line of ASCII OBIS data
    // 
    // Example:
    //  1-0:1.8.0*255(002296.8575*kWh)
    data parse_line(std::string line);

    // Parses multiple lines of ASCII OBIS data. Ignores all non relevant data.
    //
    // Example:
    //  1-0:1.8.0*255(002296.8575*kWh)
    //  1-0:0.2.0*255(ver.03,9067,20170504)
    std::vector<data> parse_lines(std::string lines);

    // Parses OBIS kwH values to double
    //
    // Example: 
    //      002296.8575*kWh
    double parse_kwh(std::string value);

}