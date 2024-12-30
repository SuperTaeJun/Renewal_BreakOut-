#pragma once
#include "pch.h"
// RoomManager Ŭ���� ����
class Room;

class RoomManager {
public:
    RoomManager() {
        for (int i = 0; i < MAX_ROOMS; ++i) {
            gameRooms.emplace_back();
        }
    }

    void AssignClientToRoom(int clientId, int roomNum) {
        std::unique_lock<std::mutex> lock(mtx);

        if (!IsValidRoomNumber(roomNum)) {
            std::cout << "Invalid room number: " << roomNum << std::endl;
            return;
        }

        gameRooms[roomNum].AddClient(clientId);
        clients[clientId].currentRoom = roomNum;
        std::cout << "Client " << clientId << " assigned to room " << roomNum << std::endl;

        //gameRooms[roomNum].IncrementReadyCount();  // Ŭ���̾�Ʈ�� �濡 ������ �غ� ���� ī��Ʈ ����
    }

    void CanClientEnterRoom(int clientId, int roomNum) {
        std::unique_lock<std::mutex> lock(mtx);
        
        if (!IsValidRoomNumber(roomNum)) {
            std::cout << "Invalid room number: " << roomNum << std::endl;
            return;
        }

        if (gameRooms[roomNum].IsFull()) {
            std::cout << "Room " << roomNum << " is full." << std::endl;
            SendLobbyPacket(clientId, false);
        }
        else {
            SendLobbyPacket(clientId, true);
        }
    }

    void EndGame(int clientId) {
        std::unique_lock<std::mutex> lock(mtx);

        CLIENT& cl = clients[clientId];
        int roomNum = cl.currentRoom;

        std::cout << "End game for room: " << roomNum << std::endl;

        for (auto& other : clients) {
            //if (other._s_id == cl._s_id) continue;

            std::unique_lock<std::mutex> stateLock(other.state_lock);
            if (ST_INGAME != other._state) continue;
            stateLock.unlock();

            if (other.currentRoom != roomNum) continue;

            // Ŭ���̾�Ʈ�� �濡�� ����
            other.currentRoom = -1;
        }

        // �� ���� �� ready_count, ingamecount �ʱ�ȭ
        gameRooms[roomNum].Clear();
    }

    Room& GetRoom(int roomNum) {
        return gameRooms[roomNum];
    }

    bool IsValidRoomNumber(int roomNum) const {
        return roomNum >= 0 && roomNum < gameRooms.size();
    }
private:
    std::vector<Room> gameRooms;
    std::mutex mtx;
};
