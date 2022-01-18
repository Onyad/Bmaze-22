#include "network/lobby.hpp"

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <string>

#include "engine/game_controller.hpp"
#include "network/session.hpp"

using EventPtr = std::unique_ptr<engine::events::Event>;
namespace network {
void Lobby::Join(std::shared_ptr<Session> session) {
  session_list_.push_back(session);
}

void Lobby::Leave(std::shared_ptr<Session> session) {
  auto it = session_list_.begin();
  while (it != session_list_.end()) {
    if ((*it).lock() == session)
      it = session_list_.erase(it);
    else
      ++it;
  }
}

void Lobby::WriteEveryone(const std::string& message) {
  using namespace std::placeholders;
  for (auto it = session_list_.begin(); it != session_list_.end();) {
    if (auto session = it->lock()) {
      session->WriteMessage(message);
      ++it;
    } else {
      it = session_list_.erase(it);
    }
  }
}

size_t Lobby::GetNumbersPlayers() const { return session_list_.size(); }

std::list<EventPtr> GameLobby::Step(EventPtr&& event) {
  return game_controller_.Step(std::move(event));
}

void GameLobby::Generate(std::unique_ptr<engine::MapGenerator> generator) {
  game_controller_ = engine::GameController(generator->operator()());
}

const engine::GameController& GameLobby::GetGameController() const {
  return game_controller_;
}
}  // namespace network
