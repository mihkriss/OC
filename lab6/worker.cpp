#include <iostream>
#include <zmq.hpp>
#include <string>
#include <map>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sstream>
#include <stdexcept>
#include "lab_api.hpp"

int main(int argc, char **argv) {
    /* argv[1] - id, argv[2] - parent_id, argv[3] - parent_port*/
    int id = std::stoi(argv[1]);
    int parent_id = std::stoi(argv[2]);
    int parent_port = std::stoi(argv[3]);

    zmq::context_t context(3); 
    zmq::socket_t parent_socket(context, ZMQ_REP); // создаем сокет, который отправляет ответы на запросы
    parent_socket.connect(get_port_name(parent_port)); // подключаем созданный сокет к тому же порту, что и переданный по аргументу

    std::map<int, zmq::socket_t> sockets; // словарь для сокетов, где
    // первый аргумент - id сокета
    // второй аргумент - сокет
    std::map<int, int> pids; // словарь для id потоков, где
    // первый аргумент - id сокета
    // второй аргумент - id потока
    std::map<int, int> ports; // словарь для портов, где
    // первый аргумент - id сокета
    // второй аргумент - порт
    std::map<std::string, int> local_dict; // локальный целочисленный словарь

    while (true) {
        int linger = 0; // задержка для закрытия сокета
        std::string request = receive_message(parent_socket); // получение сообщение
        std::istringstream command_stream(request); // создается строчный поток для удобной обработки сообщения
        std::string command;
        command_stream >> command; // считывается команда
        if (command == "pid") {
            std::string answer = "Ok:" + std::to_string(getpid());
            send_message(parent_socket, answer);
        }
        } else if (command == "create") {
            int size, new_id, port;
            command_stream >> size;
            std::vector<int> path(size);
            for (int i = 0; i < size; i++) {
                command_stream >> path[i];
            }
            command_stream >> new_id;
            if (path.empty()) {
                sockets.emplace(new_id, zmq::socket_t(context, ZMQ_REQ));
                sockets.at(new_id).setsockopt(ZMQ_SNDTIMEO, 20);
                sockets.at(new_id).setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
                port = accept_connection(sockets.at(new_id));
                int pid = fork();
                if (pid == -1) {
                    send_message(parent_socket, "Can't fork");
                    continue;
                } else if (pid == 0) {
                    create_server(new_id, id, port);
                } else {
                    ports[new_id] = port;
                    pids[new_id] = pid;
                    send_message(sockets.at(new_id), "pid");
                    send_message(parent_socket, receive_message(sockets.at(new_id)));                    
                }
            } else {
                int next_id = path.front();
                path.erase(path.begin());
                std::ostringstream msg_stream;
                msg_stream << "create " << path.size();
                for (auto it : path) {
                    msg_stream << " " << it;
                }
                msg_stream << " " << new_id;
                send_message(sockets.at(next_id), msg_stream.str());
                send_message(parent_socket, receive_message(sockets.at(next_id)));
            }
        } else if (command == "kill") {
            for (auto it = sockets.begin(); it != sockets.end(); it++) {
                send_message(it->second, "kill");
                receive_message(it->second);
                kill(pids.at(it->first), SIGTERM);
                
            }
            send_message(parent_socket, "Ok");
        } else if (command == "remove") {
            int size, remove_id;
            command_stream >> size;
            std::vector<int> path(size);
            for (int i = 0; i < size; i++) {
                command_stream >> path[i];
            }
            command_stream >> remove_id;
            if (path.empty()) { // если мы находимся в родительском узле для удаляемого
                if (sockets.count(remove_id) == 0) { 
                    send_message(parent_socket, "Error: Not found\n");
                    continue;
                }
                send_message(sockets.at(remove_id), "kill");
                std::string res = receive_message(sockets.at(remove_id));
                kill(pids.at(remove_id), SIGTERM);
                pids.erase(remove_id);
                sockets.erase(remove_id);
                ports.erase(remove_id);
                send_message(parent_socket, res);
            } else { // если мы находимся на пути к удаляемому
                auto next_id = path.front();
                path.erase(path.begin());
                std::ostringstream msg_stream;
                msg_stream << "remove " << path.size();
                for (auto it : path) {
                    msg_stream << " " << it;
                }
                msg_stream << " " << remove_id;
                send_message(sockets.at(next_id), msg_stream.str());
                send_message(parent_socket, receive_message(sockets.at(next_id)));
            }
        } else if (command == "exec") {
            int size;
            command_stream >> size;
            std::vector<int> path(size);
            for (int i = 0; i < size; i++) {
                command_stream >> path[i];
            }
            } else {
                auto next_id = path.front();
                path.erase(path.begin());
                std::ostringstream msg_stream;
                msg_stream << "exec " << path.size();
                for (auto it : path) {
                    msg_stream << " " << it;
                }
                send_message(sockets.at(next_id), msg_stream.str());
                send_message(parent_socket, receive_message(sockets.at(next_id)));
            }
        } 
    }

    return 0;
}