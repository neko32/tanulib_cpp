#pragma once
#ifndef __TLIB_CPP_COMMON_H__
#define __TLIB_CPP_COMMON_H__

#include <string>
#include <optional>

namespace tlib {

    typedef struct ProcessResult {
        std::string cmd_path;
        int result_code;
        std::optional<std::string> result;
        std::optional<std::string> error_msg;
    } ProcessResult;

}

#endif