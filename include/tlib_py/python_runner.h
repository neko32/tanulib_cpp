#pragma once
#ifndef __PYTHON_RUNNER_H__
#define __PYTHON_RUNNER_H__

#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <format>
#include "common/common.h"

namespace tlib {
    ProcessResult exec_py(std::string cmd_path, std::optional<std::vector<std::string>> params = std::nullopt) {

        std::stringstream cmd_ss;
        cmd_ss << "python3 " << cmd_path << " ";
        if(params != std::nullopt) {
            for(const std::string& param:params.value()) cmd_ss << param << " ";
        }
        std::string cmd {cmd_ss.str()};
        cmd.pop_back();

        char buffer[256];
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        if (!pipe.get()) throw std::runtime_error("popen failed!");
        try {
            while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
                result += buffer;
            }
        } catch (...) {
            throw std::runtime_error(std::format("failed to execute {}", cmd_path));
        }
        return ProcessResult {
            cmd_path,
            -1,
            result,
            std::nullopt
        };
    }
}


#endif