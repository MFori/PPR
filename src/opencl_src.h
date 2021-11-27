#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
/**
 * Created by Martin on 26.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef PPR_OPENCL_SRC_H
#define PPR_OPENCL_SRC_H

#include <string>

const std::string cl_program_src = R"CLC(

    bool valid_double(ulong value) {
        const ulong exp = 0x7FF0000000000000;
        const ulong p_zero = 0x0000000000000000;
        const ulong n_zero = 0x8000000000000000;
        bool inf_or_nan = (value & exp) == exp;
        bool sub_or_zero = (~value & exp) == exp;
        bool zero = value == p_zero || value == n_zero;

        bool normal = (!inf_or_nan && !sub_or_zero) || zero;
        return normal;
    }

    uint bucket_index(const ulong value, const uint shift, const ulong offset) {
        ulong l_index = value >> shift;
        uint index = l_index - offset;
        return index;
    }

    __kernel void get_bucket_index(__global const ulong *data, __global uint *index,
                    const ulong min, const ulong max, const uint shift, const ulong offset) {
        int i = get_global_id(0);
        ulong value = data[i];
        bool normal = valid_double(value);
        index[i] = bucket_index(value, shift, offset);
    }

    )CLC";


#endif /* PPR_OPENCL_SRC_H */

#pragma clang diagnostic pop