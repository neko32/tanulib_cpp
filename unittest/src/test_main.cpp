#include <iostream>
#include <cstdlib>
#include <vector>
#include <memory>
#include <cppunit/TestCase.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <filesystem>
#include "tlib_collection/linkedhashmap.h"
#include "chatora/chatora.h"

using namespace std;
using namespace tanu::chatora::ai;

class TanulibCppTestSuite: public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TanulibCppTestSuite);
    CPPUNIT_TEST(test_sample);
    CPPUNIT_TEST(test_linked_hashmap);
    CPPUNIT_TEST(test_ollama_chatora_talk);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }
protected:
    void test_sample();
    void test_linked_hashmap();
    void test_ollama_chatora_talk();
};

void TanulibCppTestSuite::test_sample() {
    CPPUNIT_ASSERT_EQUAL(1, 1);
}

void TanulibCppTestSuite::test_linked_hashmap() {
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
    CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(o.size()));
}

void TanulibCppTestSuite::test_ollama_chatora_talk() {
    ChatoraBase* chatora = new OllamaChatora();
    chatora->attach("http://localhost", 11434);
    chatora->deactivate_model("llama2");
    chatora->activate_model("llama2");
    string resp = chatora->talk_oneoff("田村耕一のことを教えてほしい", 0.1, 500);
    cout << resp << endl;

    delete chatora;
}


CPPUNIT_TEST_SUITE_REGISTRATION(TanulibCppTestSuite);

int main() {
    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestResultCollector collected_results;
    runner.eventManager().addListener(&collected_results);
    CppUnit::BriefTestProgressListener progress;
    runner.eventManager().addListener(&progress);
    runner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
    runner.run();

    CppUnit::CompilerOutputter outputter(&collected_results, cerr);
    outputter.write();

    return collected_results.wasSuccessful() ? 0 : 1;
}
