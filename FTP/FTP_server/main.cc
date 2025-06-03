#include "masterreactor.hpp"


int main()
{
    const int num = 4;
    master _master;
    _master.init();
    
    std::vector<std::unique_ptr<worker>> workers;
    for(int i = 0; i < num; i++){
        workers.emplace_back(std::make_unique<worker>());
    }
    _master.run();
    for(auto& _work: workers){
        _work->start();
    }
    
    for(auto& _work: workers){
        _work->join();
    }
    return 0;
}