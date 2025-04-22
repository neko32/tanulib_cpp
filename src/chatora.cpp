#include "chatora/chatora.h"
#include "tlib_unix/unix_cmd_runner.h"

#include <iostream>
#include <string>
#include <sstream>

using namespace tlib;

namespace tanu::chatora::ai {

    OllamaShowResponse OllamaChatora::show(const std::string& name) {
        OllamaShowResponse resp;
        ProcessResult out = exec_cmd_unix(std::format("ollama show {}", name));
        std::stringstream ss {out.result.value()};
        std::string line{};
        std::vector<std::string> params{};
        std::vector<License> licenses{};
        while(getline(ss, line)) {
            if(line.starts_with("    architecture")) {
                resp.model_architecture = line;
            } else if(line.starts_with("    parameters")) {
                resp.model_parameters = line;
            } else if(line.starts_with("    context length")) {
            //    resp.context_length = std::stoull(line);
                resp.context_length = 100;
            } else if(line.starts_with("    embedding length")) {
            //    resp.embedding_length = std::stoull(line);
                resp.embedding_length = 100;
            } else if(line.starts_with("    quantization")) {
                resp.quantization = line;
            } else if(line.starts_with("  Parameters")) {
                while(getline(ss, line) && line != "") {
                    params.push_back(line);
                }
            } else if(line.starts_with("  License")) {
                while(getline(ss, line)) {
                    licenses.emplace_back(line);
                }
            }
        }
        resp.parameters = params;
        resp.licenses = licenses;

        return resp;
    }

    std::vector<LLMModel> OllamaChatora::get_models() {
        std::vector<LLMModel> models{};
        ProcessResult out = exec_cmd_unix("ollama list|awk \"NR!=1{print $1}\"");
        std::cout << out.result.value() << std::endl;
        std::string tmp_s{};
        std::stringstream ss{out.result.value()}; 
        std::string name;
        std::string ver;
        std::string namever;
        while(getline(ss, tmp_s)) {
            auto loc = tmp_s.find_first_of(' ');
            namever = tmp_s.substr(0, loc);
            loc = namever.find_first_of(":");
            name = namever.substr(0, loc);
            ver = namever.substr(loc + 1);
            
            OllamaShowResponse show_resp = this->show(name);
            models.emplace_back(
                name, ver, static_cast<unsigned long long>(100), show_resp.model_architecture, 
                // show_resp.model_parameters,
                100,
                show_resp.context_length, show_resp.embedding_length, static_cast<double>(4), 
                show_resp.capabilities, show_resp.licenses
            );
            std::cout << show_resp.context_length << "," << show_resp.quantization << std::endl;
        }
        
        return models;
    }

    const std::string LLMModel::capabilities_as_str() const {
        std::stringstream ss;
        ss << "[";
        for(const std::string& capability:this->m_capabilities) {
            ss << capability << ",";
        }
        ss << "]";
        std::string t = ss.str();
        t.pop_back();
        return t;
    }

    const std::string LLMModel::licenses_as_str() const {
        std::stringstream ss;
        ss << "[";
        for(const License& license:this->m_licenses) {
            ss << license.str() << ",";
        }
        ss << "]";
        std::string t = ss.str();
        t.pop_back();
        return t;
    }

    const std::string LLMModel::str() const {
        std::stringstream ss;
        ss << std::format("{{name:{}, version:{}, size:{}, architecture:{}, \
            context length:{}, embedding length:{}, quantization:{}, \
            capabilities:{}, licenses:{}}}",
            this->m_name, this->m_version, this->m_size, this->m_architecture, 
            this->m_context_length, this->m_embedding_length, this->quantization_as_str(),
            this->capabilities_as_str(), this->licenses_as_str());
        return ss.str();
    }

    const LLMModel OllamaChatora::get_active_model() {
        
    }
}
