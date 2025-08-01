#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include <any>
#include <typeindex>
#include <syncstream> // C++20, ulepszony w C++23
#include <latch>      // C++20, ulepszony w C++23

class EventBus {
public:
    using Handler = std::function<void(const std::any&)>;
    
    // Rejestracja handlera dla typu zdarzenia
    template <typename EventType>
    void subscribe(std::function<void(const EventType&)> handler) {
        std::scoped_lock lock(mutex_);
        handlers_[typeid(EventType)].push_back(
            [handler](const std::any& event) {
                handler(std::any_cast<const EventType&>(event));
            }
        );
    }

    // Wysyłanie zdarzenia (synchronicznie)
    template <typename EventType>
    void publish(const EventType& event) {
        std::vector<Handler> local_handlers;
        
        {
            std::scoped_lock lock(mutex_);
            if (auto it = handlers_.find(typeid(EventType)); it != handlers_.end()) {
                local_handlers = it->second;
            }
        }
        
        for (auto& handler : local_handlers) {
            handler(event);
        }
    }

    // Wysyłanie zdarzenia asynchronicznie
template <typename EventType>
void post(EventType&& event) {
    std::scoped_lock lock(queue_mutex_);
    event_queue_.emplace(std::forward<EventType>(event)); // Emplace dla przeniesienia
    queue_cv_.notify_all(); // Użyj notify_all dla wielu workerów
}

    // Uruchomienie workerów
    void start_workers(unsigned num_workers) {
        stop_requested_ = false;
        workers_.reserve(num_workers);
        
        for (unsigned i = 0; i < num_workers; ++i) {
            workers_.emplace_back([this, i] {
                worker_thread(i);
            });
        }
    }

    // Zatrzymanie workerów
    void stop_workers() {
        {
            std::scoped_lock lock(queue_mutex_);
            stop_requested_ = true;
        }
        queue_cv_.notify_all();
        
        for (auto& worker : workers_) {
            if (worker.joinable()) worker.join();
        }
    }

private:
    void worker_thread(unsigned id) {
    while (true) {
        std::any event;
        {
            std::unique_lock lock(queue_mutex_);
            queue_cv_.wait(lock, [this] { return !event_queue_.empty() || stop_requested_; });
            if (stop_requested_ && event_queue_.empty()) return;
            event = std::move(event_queue_.front()); // Przenoszenie
            event_queue_.pop();
        }
        try {
                // Znajdź i wykonaj odpowiednie handlery
                std::vector<Handler> local_handlers;
                {
                    std::scoped_lock lock(mutex_);
                    auto it = handlers_.find(std::type_index(event.type()));
                    if (it != handlers_.end()) {
                        local_handlers = it->second;
                    }
                }
                
                for (auto& handler : local_handlers) {
                    handler(event);
                }
            } catch (const std::bad_any_cast& e) {
                std::osyncstream(std::cerr) 
                    << "Worker " << id << ": " << e.what() << '\n';
            }  catch (const std::exception& e) { 
                std::osyncstream(std::cerr) 
                    << "Worker " << id << ": Exception occurred: " << e.what() << '\n';
            } catch (...) {
                std::osyncstream(std::cerr) 
                    << "Worker " << id << ": Unknown exception occurred.\n";
            }
        std::osyncstream(std::cout) 
            << "Worker " << id << ": Processed event of type " 
            << event.type().name() << "\n";
        }
    }

private:
    std::unordered_map<std::type_index, std::vector<Handler>> handlers_;
    std::queue<std::any> event_queue_;
    std::vector<std::jthread> workers_;
    std::mutex mutex_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    std::atomic<bool> stop_requested_ {false};
};

// Przykładowe zdarzenia
struct UserLoginEvent { std::string username; };
struct DataUpdateEvent { int new_value; };

int main() {
    EventBus bus;
    std::latch completion_latch(3); // C++20/23 synchronizacja

    // Rejestracja handlerów
    bus.subscribe<UserLoginEvent>([&](const UserLoginEvent& e) {
        std::osyncstream(std::cout) << "Obsługa logowania: " << e.username << "\n";
        completion_latch.count_down();
    });
    
    bus.subscribe<DataUpdateEvent>([&](const DataUpdateEvent& e) {
        std::osyncstream(std::cout) << "Aktualizacja danych: " << e.new_value << "\n";
        completion_latch.count_down();
    });

    // Uruchomienie workerów
    bus.start_workers(4);

    // Wysyłanie zdarzeń (rôzne wątki)
    std::jthread t1([&] {
        bus.post(UserLoginEvent{"admin"});
        bus.post(UserLoginEvent{"guest"});
    });
    
    std::jthread t2([&] {
        bus.post(DataUpdateEvent{42});
    });

    // Oczekiwanie na zakończenie przetwarzania
    completion_latch.wait();
    
    // Czekaj chwilę przed zatrzymaniem
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    bus.stop_workers();

    return 0;
}