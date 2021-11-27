#ifdef ide
#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-branch-clone"
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
#endif
/**
 * Created by Martin on 26.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef PPR_OPENCL_SRC_H
#define PPR_OPENCL_SRC_H

#include <string>

/**
 * OpenCl program source code
 */
const std::string cl_program_src = R"CLC(

    __constant const ulong d_exp = 0x7FF0000000000000;
    __constant const ulong p_zero = 0x0000000000000000;
    __constant const ulong n_zero = 0x8000000000000000;

    bool valid_double(ulong value) {
        bool inf_or_nan = (value & d_exp) == d_exp;
        bool sub_or_zero = (~value & d_exp) == d_exp;
        bool zero = value == p_zero || value == n_zero;

        bool normal = (!inf_or_nan && !sub_or_zero) || zero;
        return normal;
    }

    bool in_range_double(const ulong value, const ulong min, const ulong max) {
        return value >= min && value <= max;
    }

    uint bucket_index(const ulong value, const uint shift, const ulong offset) {
        ulong l_index = value >> shift;
        uint index = l_index - offset;
        return index;
    }

    __kernel void get_bucket_index(__global const ulong *data, __global uint *index, __global bool *flag,
                    const ulong min, const ulong max, const uint shift, const ulong offset) {
        int i = get_global_id(0);
        ulong value = data[i];

        if (valid_double(value) && in_range_double(value, min, max)) {
            index[i] = bucket_index(value, shift, offset);
            flag[i] = 1;
        } else {
            index[i] = 0;
            flag[i] = 0;
        }
    }

    )CLC";


#endif /* PPR_OPENCL_SRC_H */

#ifdef ide
#pragma clang diagnostic pop
#endif
