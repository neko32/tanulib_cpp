#pragma once
#ifndef __CHATORA_H__
#define __CHATORA_H__

#include <string>
#include <vector>
#include <format>
#include <optional>
#include <map>

namespace tanu::chatora::ai {

    class License {
    private:
        std::string m_name;
    public:
        License(std::string name):m_name(name){}
        const std::string name() const {return m_name;}
        const std::string str() const {return format("{{name:{}}}", m_name);}
    };

    class LLMModel {
    private:
        std::string m_name;
        std::string m_version;
        unsigned long long m_size;
        std::string m_architecture;
        unsigned long long m_num_params;
        unsigned long long m_context_length;
        unsigned long long m_embedding_length;
        int m_quantization;
        std::vector<std::string> m_capabilities;
        std::vector<License> m_licenses;

    public:
        LLMModel(
            std::string name,
            std::string version,
            unsigned long long size,
            std::string architecture,
            unsigned long long num_params,
            unsigned long long context_length,
            unsigned long long embedding_length,
            double quantization,
            std::vector<std::string> capabilities,
            std::vector<License> licenses):
                m_name(name),
                m_version(version),
                m_size(size),
                m_architecture(architecture), 
                m_num_params(num_params),
                m_context_length(context_length),
                m_embedding_length(embedding_length),
                m_quantization(quantization),
                m_capabilities(capabilities),
                m_licenses(licenses) {}
        
        LLMModel(const LLMModel& other):
                m_name(other.m_name),
                m_version(other.m_version),
                m_size(other.m_size),
                m_architecture(other.m_architecture), 
                m_num_params(other.m_num_params),
                m_context_length(other.m_context_length),
                m_embedding_length(other.m_embedding_length),
                m_quantization(other.m_quantization),
                m_capabilities(other.m_capabilities),
                m_licenses(other.m_licenses) {}

        const std::string name() const {return m_name;};
        const std::string version() const {return m_version;};
        unsigned long long size() const {return m_size;};
        const std::string architecture() const {return m_architecture;};
        unsigned long long num_params() const {return m_num_params;};
        unsigned long long context_length() const {return m_context_length;};
        unsigned long long embedding_length() const {return m_embedding_length;};
        double quantization() const {return m_quantization;};
        std::string quantization_as_str() const {return std::format("Q{}", m_quantization);}
        std::vector<std::string> capabilities() const {return m_capabilities;};
        const std::string capabilities_as_str() const;
        std::vector<License> licenses() const {return m_licenses;};
        const std::string licenses_as_str() const;
        const std::string str() const;
    };

    class ChatoraBase {
    private:
        std::map<std::string, LLMModel> m_models;
        std::optional<std::string> m_active_model_name;
        std::optional<std::string> m_model_server_host;
        std::optional<int> m_model_server_port;
    public:
        ChatoraBase(){}
        virtual ~ChatoraBase(){}
        virtual void add_model(LLMModel model){ this->m_models.insert({model.name() + ":" + model.version(), model});}
        virtual bool attach(const std::string& model_server_host, const int model_server_port);
        virtual const std::string talk_oneoff(const std::string& prompt, double temperature, int max_tokens);
        virtual bool activate_model(const std::string& model_name) = 0;
        virtual bool deactivate_model(const std::string& model_name) = 0;
        virtual std::vector<LLMModel> get_models() = 0;
        virtual const std::vector<LLMModel> get_active_models() = 0;
    };

    struct OllamaShowResponse {
        std::string model_architecture;
        std::string model_parameters;
        unsigned long long context_length;
        unsigned long long embedding_length;
        std::string quantization;
        std::vector<std::string> capabilities;
        std::vector<std::string> parameters;
        std::vector<License> licenses;
    };

    class OllamaChatora:public ChatoraBase {
    private:
        OllamaShowResponse show(const std::string& name);
    public:
        OllamaChatora(): ChatoraBase(){}
        std::vector<LLMModel> get_models() override;
        const std::vector<LLMModel> get_active_models() override;
        bool attach(const std::string& model_server_host, const int model_server_port) override;
        virtual bool activate_model(const std::string& model_name) override;
        virtual bool deactivate_model(const std::string& model_name) override;
    };

}

#endif