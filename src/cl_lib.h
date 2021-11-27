/**
 * Created by Martin on 26.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 *
 * Used to include OpenCl in other project files
 */
#ifndef PPR_CL_LIB_H
#define PPR_CL_LIB_H

#define CL_TARGET_OPENCL_VERSION 200
#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_LOG_ERRORS stdout
#pragma comment(lib, "OpenCL.lib")

#include <CL/cl.h>
#include <CL/opencl.hpp>

#endif /* PPR_CL_LIB_H */
