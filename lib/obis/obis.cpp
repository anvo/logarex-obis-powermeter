/*
 * Copyright (c) 2022 anvo
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */
#include "obis.h"

#include <cstdlib>

namespace obis {
    
    // Parse a single line of ASCII OBIS data
    // 
    // Example:
    //  1-0:1.8.0*255(002296.8575*kWh)
    data parse_line(std::string line) {
        std::size_t start_code = line.find_first_of(":");
        std::size_t end_code = line.find_first_of("*");

        if (start_code == std::string::npos || end_code == std::string::npos) {
            return data {};
        }

        std::string code = line.substr(start_code +1, end_code - start_code - 1);

        std::size_t start_value = line.find_first_of("(");
        std::size_t end_value = line.find_last_of(")");
        std::string value = line.substr(start_value + 1, end_value - start_value -1);

        return data{
            .code = code,
            .value = value
        };
    }

    // Parses multiple lines of ASCII OBIS data. Ignores all non relevant data.
    //
    // Example:
    //  1-0:1.8.0*255(002296.8575*kWh)
    //  1-0:0.2.0*255(ver.03,9067,20170504)
    std::vector<data> parse_lines(std::string lines) {
        std::vector<data> retval;
        std::size_t pos = 0;
        while(pos != std::string::npos) {
            std::size_t next = lines.find('\n', pos+1);
            std::string line = lines.substr(pos, next-pos);

            if(line.find("1-0") != std::string::npos) {
                retval.push_back(parse_line(line));
            }
            pos = next;
        }

        return retval;
    }

    // Parses OBIS kwH values to double
    //
    // Example: 
    //      002296.8575*kWh
    double parse_kwh(std::string value) {
        return std::atof(value.c_str());
    }
}