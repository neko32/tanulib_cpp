#include "chatora/chatora.h"
#include "tlib_unix/unix_cmd_runner.h"

#include <iostream>
#include <string>
#include <sstream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace tlib;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

namespace tanu::chatora::ai {

    bool ChatoraBase::attach(const std::string& model_server_host, const int model_server_port) {
        this->m_model_server_host = model_server_host;
        this->m_model_server_port = model_server_port;

        return true;
    }

    const std::string ChatoraBase::talk_oneoff(const std::string& prompt, double temperature, int max_tokens) {

        if(this->m_model_server_host == std::nullopt) {
            throw std::runtime_error("not yet connected to the model server!");
        }

        json::value post_req_body;
        post_req_body["model"] = json::value::string("llama2");
        post_req_body["prompt"] = json::value::string(prompt);
        post_req_body["stream"] = json::value::boolean(false);
        post_req_body["temperature"] = json::value::number(temperature);
        post_req_body["max_tokens"] = json::value::number(max_tokens);

        return pplx::create_task([&] {
            http_client cl(std::format("{}:{}/api/generate", this->m_model_server_host.value(), this->m_model_server_port.value()));
            return cl.request(methods::POST, "", post_req_body.serialize(), "application/json");
        })
        .then([](http_response resp) {
            if(resp.status_code() == status_codes::OK) {
                return resp.extract_json();
            }
        })
        .then([](json::value js_resp) {
            return js_resp["response"].as_string();
        }).get();
    }

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
