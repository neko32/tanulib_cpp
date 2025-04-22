#include "tlib_collection/linkedhashmap.h"
#include "tlib_py/python_runner.h"
#include "chatora/chatora.h"

#include <iostream>
using namespace std;
using namespace tanu::chatora::ai;

int main() {

    /*
    tlib::LinkedHashMap<int, int> m;
    m.put(1, 32);
    m.put(5, 10);
    m.put(3, 40);
    cout << m.str() << endl;

    tlib::LinkedHashMap<int, string> n;
    n.put(1, "takori");
    n.put(5, "ikari");
    n.put(3, "pokori");
    n.put(10, "ikari");
    cout << n.str() << endl;

    tlib::LinkedHashMap<string, int> o;
    o.put("takori", 1);
    o.put("ikari", 3);
    o.put("pokori", 5);
    o.put("ikari", 10);
    o.remove("pokori");
    cout << o.str() << endl;
    cout << o.size() << endl;

    tlib::ProcessResult rez = tlib::exec_py("/home/neko32/dev/python/pystudy/main.py");
    cout << rez.result.value() << endl;
    */

    OllamaChatora oc;
    vector<LLMModel> models = oc.get_models();
    for(const auto& model:models) cout << model.str() << endl;

}
