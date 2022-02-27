#include "lab_api.hpp"
#include <string.h> // for strdup
#include <unistd.h> // for execv

void create_server(int id, int parent_id, int port) {
    char *arg1 = strdup((std::to_string(id)).c_str()); // выделяется память под строку и эта строка копируется в выделенную память
    char *arg2 = strdup((std::to_string(parent_id)).c_str());
    char *arg3 = strdup((std::to_string(port)).c_str());
    char *args[] = {"./worker", arg1, arg2, arg3, NULL};
    execv("./worker", args);
    free(arg1);
    free(arg2);
    free(arg3);
}

bool send_message(zmq::socket_t &socket, const std::string &message_string) {
    zmq::message_t message(message_string.size());
    memcpy(message.data(), message_string.c_str(), message_string.size());
    return socket.send(message);
}

std::string receive_message(zmq::socket_t &socket) {
    zmq::message_t message;
    bool result_recv;
    try {
        result_recv = socket.recv(&message); // true, если сообщение получено
        // false, если сообщение не получено
    } catch (...) { // false, если ошибка
        result_recv = false;
    }

    std::string message_str(static_cast<char *>(message.data()), message.size());
    if (message_str.empty() || !result_recv) {
        return "Error: Node is not available";
    }   
    return message_str;
}

std::string get_port_name(int port) {
    return "tcp://127.0.0.1:" + std::to_string(port);
}

int accept_connection(zmq::socket_t &socket) {
    int port = 4040;
    while (true) {
        try {
            socket.bind(get_port_name(port)); // bind, т.к. привязывается к долгоживущему порту
            break;
        } catch (...) {
            port++;
        }
    }

    return port;
}