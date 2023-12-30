#include <entt/entt.hpp>
#include <iostream>

#include <chrono>
#include <ctime>
#include <cmath>

struct Position {
    float x;
    float y;
};

struct Velocity {
    float dx;
    float dy;
};



class Timer
{
public:
    void start()
    {
        m_StartTime = std::chrono::system_clock::now();
        m_bRunning = true;
    }

    void stop()
    {
        m_EndTime = std::chrono::system_clock::now();
        m_bRunning = false;
    }

    double elapsedMilliseconds()
    {
        std::chrono::time_point<std::chrono::system_clock> endTime;

        if(m_bRunning)
        {
            endTime = std::chrono::system_clock::now();
        }
        else
        {
            endTime = m_EndTime;
        }

        return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_StartTime).count();
    }

    double elapsedSeconds()
    {
        return elapsedMilliseconds() / 1000.0;
    }

private:
    std::chrono::time_point<std::chrono::system_clock> m_StartTime;
    std::chrono::time_point<std::chrono::system_clock> m_EndTime;
    bool                                               m_bRunning = false;
};


void update(entt::registry &registry) {
    auto view = registry.view<const Position, Velocity>();

    // use a callback
    //view.each([](const auto &pos, auto &vel) { /* ... */ });

    // use an extended callback
    //view.each([](const auto entity, const auto &pos, auto &vel) { /* ... */ });

    // use a range-for
    //for(auto [entity, pos, vel]: view.each()) {

    //}

    // use forward iterators and get only the components of interest
    //for(auto entity: view) {
    //    auto &vel = view.get<velocity>(entity);

    //}
}

int main() {
    entt::registry registry;

    entt::observer observer {registry,entt::collector.update<Position>()};

    entt::entity firstEntity;

    Timer fullTime;
    Timer createTimer;
    Timer iterateTimer;

    fullTime.start();
    createTimer.start();
    for (int i=0; i<1; i++) {
        auto entity = registry.create();
        if (i==0) {
            firstEntity = entity;
        }

        registry.emplace<Position>(entity, Position{0,0});
        registry.emplace<Velocity>(entity, Velocity{i*0.1f,1});
    }
    createTimer.stop();

    iterateTimer.start();
    auto view = registry.view<Position, const Velocity>();

    view.each([&registry] (entt::entity entity, Position& pos, const Velocity& velocity) {
        pos.x += velocity.dx;
        pos.y += velocity.dy;


        auto& patched = registry.patch<Position>(entity);

        if (&patched == &pos) {
            std::cout << "Same \n";
        }


    });



    /*for (auto entity : view) {

        auto& pos = registry.get<Position>(entity);
        const auto& velocity = registry.get<Velocity>(entity);

        pos.x += velocity.dx;
        pos.y += velocity.dy;

        //if (pos.x>10) {
        //    registry.patch<Position>(entity);
        //}
    }
    */

    /*for(auto [entity, pos, velocity]: view.each()) {
        pos.x += velocity.dx;
        pos.y += velocity.dy;
    }
    */


    iterateTimer.stop();

    fullTime.stop();

    std::cout << "Total : " << fullTime.elapsedMilliseconds() << "ms \n";
    std::cout << "Create : " << createTimer.elapsedMilliseconds() << "ms \n";
    std::cout << "Iterate : " << iterateTimer.elapsedMilliseconds() << "ms \n";


    int count = 0;
    for (auto changedEntity : observer) {
        count++;


        //std::cout <<  "Changed " << entt::to_integral(changedEntity)<<" - " << registry.get<Position>(changedEntity).x << "\n";
    }

    observer.clear();

    std::cout << "Num changed entities " << count << "\n";

    std::cout << registry.get<Position>(firstEntity).x << "\n";
    std::cout << registry.get<Position>(firstEntity).y << "\n";


}