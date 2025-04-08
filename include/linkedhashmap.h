#pragma once
#ifndef __LINKEDHASHMAP_H__
#define __LINKEDHASHMAP_H__


#include <map>
#include <list>
#include <stdexcept>
#include <sstream>

namespace tlib {

    template <typename K, typename V>
    class LinkedHashMap {
    private:
        std::map<K, typename std::list<std::pair<K, V>>::iterator> m_map;
        std::list<std::pair<K, V>> m_order;

    public:
        void put(const K& key, const V& value) {
            if (m_map.find(key) != m_map.end()) {
                m_order.erase(m_map[key]);
            }
            m_order.push_back({key, value});
            m_map[key] = --m_order.end();
        }

        V get(const K& key) {
            if (m_map.find(key) == m_map.end()) {
                throw std::out_of_range("Key not found");
            }
            return m_map[key]->second;
        }

        bool contains(const K& key) {
            return m_map.find(key) != m_map.end();
        }

        void remove(const K& key) {
            auto it = m_map.find(key);
            if(it != m_map.end()) {
                m_order.erase(it->second);
                m_map.erase(it);
            }
        }

        size_t size() const {
            return m_map.size();
        }

        std::string str() const {
            std::stringstream ss;
            ss << "{";
            for (const auto& pair : m_order) {
                ss << pair.first << ":" << pair.second << ",";
            }
            std::string t = ss.str();
            t.pop_back();
            t.push_back('}');
            return t;
        }
    };

}


#endif