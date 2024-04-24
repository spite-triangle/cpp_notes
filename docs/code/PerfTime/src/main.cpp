#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include <Windows.h>

#include <iostream>
#include <thread>
#include <string>

#include "PerfTime.h"

TEST_CASE("打点测试"){
    OwO::StartHit("test111",OwO_POS);
    Sleep(100);
    OwO::EndHit("test111");
    OwO::StartHit("test1",OwO_POS);
    Sleep(100);
    OwO::EndHit("test1");
    printf("%s\n" ,OwO::Report().c_str());
}

TEST_CASE("分组测试"){
    OwO::StartHit("test",OwO_POS, {"a","b","c"});
    Sleep(100);
    OwO::EndHit("test");
    printf("%s\n" ,OwO::Report().c_str());
}

TEST_CASE("多次展示报告测试"){
    for (size_t i = 0; i < 3; i++)
    {
        printf("=========== %d ===========\n", i);
        OwO::StartHit("test",OwO_POS);
        Sleep(100);
        OwO::EndHit("test");
        printf("%s\n" ,OwO::Report("test.csv").c_str());
    }
}

TEST_CASE("多线程测试"){
    auto fcn = [](){
        for (size_t i = 0; i < 10; i++)
        {
            OwO::StartHit("test" ,OwO_POS,{"a","b","c"});
            Sleep(100);
            OwO::EndHit("test");
        }
    };

    std::vector<std::thread> vec;
    for (size_t i = 0; i < 10; i++)
    {
        vec.emplace_back(std::move(std::thread(fcn)));
    }

    for (size_t i = 0; i < 10; i++)
    {
        vec[i].join();
    }
    
    printf("%s\n" ,OwO::Report().c_str());
}

std::vector<double> Fcn(const std::string & strMark, const OwO::Dataframe & dataFrame){
    std::vector<double> res(2);
    res[0] = 1;
    res[1] = 2;
    return res;
}

TEST_CASE("自定输出测试"){
    OwO::AppendReport({"a","b"}, Fcn);

    for (size_t i = 0; i < 3; i++)
    {
        printf("888888888888888  %d 888888888888888\n", i);
        OwO::StartHit("test", OwO_POS,{"a","b","c"});
        Sleep(100);
        OwO::EndHit("test");
        printf("%s\n" ,OwO::Report().c_str());
    }
}

TEST_CASE("测试block"){
    {
        OwO::HitBlock clsBlock("tetst",OwO_POS,{"aa"});
        Sleep(100);
    }
    printf("%s\n" ,OwO::Report().c_str());
}

TEST_CASE("测试宏"){
    OwO_START("test",{"a"});
    Sleep(100);
    OwO_END("test");

    OwO_BLOCK("test1"){
        Sleep(100);
    }
    
    {
        OwO_BLOCK_MARK("test2");
        Sleep(100);
    }

    printf("%s\n" ,OwO::Report().c_str());
}
TEST_CASE("测试多标签"){
    {OwO::HitBlock clsBlock("test1",OwO_POS, {"aaa"});Sleep(10);}
    {OwO::HitBlock clsBlock("test2",OwO_POS, {"aaa"});Sleep(10);}
    {OwO::HitBlock clsBlock("test3",OwO_POS, {"aaa"});Sleep(10);}
    {OwO::HitBlock clsBlock("test4",OwO_POS, {"aaa"});Sleep(10);}
    {OwO::HitBlock clsBlock("test5",OwO_POS, {"aaa"});Sleep(10);}
    {OwO::HitBlock clsBlock("test6",OwO_POS, {"aaa"});Sleep(10);}
    {OwO::HitBlock clsBlock("test7",OwO_POS, {"aaa"});Sleep(10);}
    {OwO::HitBlock clsBlock("test8",OwO_POS, {"aaa"});Sleep(10);}
    {OwO::HitBlock clsBlock("test9",OwO_POS, {"aaa"});Sleep(10);}
    {OwO::HitBlock clsBlock("test10",OwO_POS, {"aaa"});Sleep(10);}
    {OwO::HitBlock clsBlock("test11",OwO_POS, {"aaa"});Sleep(10);}
    {OwO::HitBlock clsBlock("test12",OwO_POS, {"aaa"});Sleep(10);}
    {OwO::HitBlock clsBlock("test13",OwO_POS, {"aaa"});Sleep(10);}
    {OwO::HitBlock clsBlock("test14",OwO_POS, {"aaa"});Sleep(10);}

    printf("%s\n" ,OwO::Report().c_str());
}


int main(int argc, char *argv[])
{
    doctest::Context context;
    context.applyCommandLine(argc, argv);
    return context.run();
}

