#pragma once
#include "pch.h"


class Room {
public:
    Room() : ready_count(0), ingamecount(0) {}

    // ���� ������ ����
    Room(const Room& other)
        : players(other.players),
        ready_count(other.ready_count.load()),  // atomic ������ ���� load�Ͽ� ����
        ingamecount(other.ingamecount.load())   // atomic ������ ���� load�Ͽ� ����
    {}

    // ���� ������ ����
    Room& operator=(const Room& other) {
        if (this != &other) {
            players = other.players;
            ready_count.store(other.ready_count.load());  // atomic ���� ����
            ingamecount.store(other.ingamecount.load());  // atomic ���� ����
        }
        return *this;
    }

    void AddClient(int clientId) {
        players.push_back(clientId);
    }

    void RemoveClient(int clientId) {
        players.erase(std::remove(players.begin(), players.end(), clientId), players.end());
    }

    bool IsFull() const {
        cout << "������ ? " << players.size() << endl;
        return players.size() >= ENTER_CLIENT;
    }

    void Clear() {
        players.clear();
        ready_count = 0;
        ingamecount = 0;
    }

    void IncrementReadyCount() {
        ++ready_count;
    }

    void DecrementReadyCount() {
        --ready_count;
    }

    void IncrementInGameCount() {
        ++ingamecount;
    }

    void DecrementInGameCount() {
        --ingamecount;
    }

    void ResetReadyCount() {
        ready_count = 0;
    }

    std::atomic<int>& GetReadyCount() {
        return ready_count;
    }

    std::atomic<int>& GetInGameCount() {
        return ingamecount;
    }

    int GetPlayerCount() const {
        return players.size();
    }

    const std::vector<int>& GetPlayers() const {
        return players;
    }

private:
    std::vector<int> players;
    std::atomic<int> ready_count;  // �غ� ������ �÷��̾� ��
    std::atomic<int> ingamecount;  // ���� ���� �÷��̾� ��
};
