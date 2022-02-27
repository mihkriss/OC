#include <iostream>
#include <string>
#include <map>
#include <set>
#include <cctype>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <zmq.hpp>
#include "tree.hpp"
#include "lab_api.hpp"

int main() {
    std::string command; 
    TTree topology; 
    int child_pid; 
    zmq::context_t context(1); 
    std::map<int, zmq::socket_t> sockets; // словарь для сокетов, где
    // первый аргумент - id сокета
    // второй аргумент - сокет
    std::map<int, int> pids; // словарь для id потоков, где
    // первый аргумент - id сокета
    // второй аргумент - id потока
    std::map<int, int> ports; // словарь для портов, где
    // первый аргумент - id сокета
    // второй аргумент - порт

    while (std::cin >> command) {
        std::string res; 
        int linger = 0; // задержка после закрытия сокета
        if (command == "create" || command == "c") {
            int new_id, parent_id; 
            std::cin >> new_id >> parent_id;
            if (parent_id == -1 && pids.count(new_id) == 0) { // добавление вычислительного узла к управляющему, т.е. к корню
                sockets.emplace(new_id, zmq::socket_t(context, ZMQ_REQ)); // в словарь вставляется запрашивающий сокет с id new_id
                sockets.at(new_id).setsockopt(ZMQ_LINGER, &linger, sizeof(linger)); 
                // для задержки после закрытия сокета, чтобы сообщения не отбрасывались в течение linger
                sockets.at(new_id).setsockopt(ZMQ_SNDTIMEO, 20); // для нового сокета устанавливается опция zmq_sndtimeo
                // для определения интервала ожидания отправки сообщения, если сообщение за этот период не будет отправлено, то вернется ошибка eagain
                int port = accept_connection(sockets.at(new_id)); // происходит привязка сокета с id new_id к некоторому порту
                child_pid = fork();
                if (child_pid == -1) {
                    std::cout << "Unable to create worker node.\n";
                    exit(1);
                } else if (child_pid == 0) { // дочерний процесс
                    create_server(new_id, parent_id, port);
                } else {
                    ports[new_id] = port;
                    pids[new_id] = child_pid;
                    
                    send_message(sockets.at(new_id), "pid");
                    res = receive_message(sockets.at(new_id));
                }
            } else {
                if (topology.Search(new_id) != nullptr) { // если уже есть узел с id new_id
                    std::cout << "Error: Already exists\n";
                    continue;
                }
                if (topology.Search(parent_id) == nullptr) { // если нет родителя с нужным id
                    std::cout << "Error: Parent not found\n";
                    continue;
                }
                auto path = topology.GetPath(parent_id); // получаем вектор пар, путь до нужной вершины. Первый элемент пары - id узла, второй - направление движения по пути (либо к сыну, либо к брату)
                std::vector<int> path_sons; // вектор id узлов, из которых затем происходит движение по сыну
                for (auto it : path) {
                    if (it.second == TO_SON) {
                        path_sons.push_back(it.first);
                    }
                }
                int id_on_path = path_sons.front();
                path_sons.erase(path_sons.begin());
                std::ostringstream msg_stream; 
                msg_stream << "create " << path_sons.size();
                for (auto it : path_sons) {
                    msg_stream << " " << it;
                }
                msg_stream << " " << new_id;
                send_message(sockets.at(id_on_path), msg_stream.str());
                res = receive_message(sockets.at(id_on_path));
            }
            if (res.substr(0, 2) == "Ok") {
                topology.Insert(new_id, parent_id);
            }
            std::cout << res << "\n";
        } else if (command == "remove" || command == "r") {
            int remove_id;
            std::cin >> remove_id;
            if (pids.size() == 0) { 
                std::cout << "Error: Not found\n";
                continue;
            }
            if (pids.count(remove_id) != 0) { // если удаляемый вычислительный узел является ребенком управляющего
                send_message(sockets.at(remove_id), "kill");
                res = receive_message(sockets.at(remove_id));
                kill(pids.at(remove_id), SIGTERM); // SIGTERM - сигнал завершения процесса. Происходит запрос остановки работы процесса.
                pids.erase(remove_id);
                sockets.erase(remove_id);
                ports.erase(remove_id);
                if (res.substr(0, 2) == "Ok") {
                    topology.Remove(remove_id);
                }
                std::cout << res << "\n";
                continue;
            }
            // иначе
            auto path = topology.GetPath(remove_id); // получаем путь до удаляемой вершины
            if (path.empty()) {
                std::cout << "Error: Not found\n";
                continue;
            }
            std::vector<int> path_sons;
            for (auto it : path) {
                if (it.second == TO_SON) {
                    path_sons.push_back(it.first);
                }
            }
            int id_on_path = path_sons.front();
            path_sons.erase(path_sons.begin());
            path_sons.pop_back(); // выбрасываем из пути удаляемый элемент
            std::ostringstream msg_stream;
            msg_stream << "remove " << path_sons.size();
            for (auto it : path_sons) {
                msg_stream << " " << it;
            }
            msg_stream << " " << remove_id;
            send_message(sockets.at(id_on_path), msg_stream.str());
            res = receive_message(sockets.at(id_on_path));
            if (res.substr(0, 2) == "Ok") {
                topology.Remove(remove_id);
            }
            std::cout << res << "\n";
        } else if (command == "exec") {
            int id, value;
            std::string name, help_str, id_str, value_str;
            bool find = true;
            std::cin.ignore(256, ' ');
            std::getline(std::cin, help_str);
            int i;
            for (i = 0; i < help_str.size(); i++) { // считывание id
                if (std::isdigit(help_str[i])) {
                    id_str += help_str[i];
                } else if (help_str[i] == ' ') {
                    if (id_str.empty()) {
                        continue;
                    } else {
                        id = std::stoi(id_str);
                        break;
                    }
                } else { // no digit
                    break;
                }
            }
            for (; i < help_str.size(); i++) { // считывание name
                if (std::isdigit(help_str[i]) || std::isalpha(help_str[i]) || help_str[i] == '+') {
                    name += help_str[i];
                } else if (help_str[i] == ' ') {
                    if (name.empty()) {
                        continue;
                    } 
                    break;
                } else { // неподходящий символ
                    break;
                }
            }
            for (; i < help_str.size(); i++) { // считывание value
                if (std::isdigit(help_str[i])) {
                    value_str += help_str[i];
                } else if (help_str[i] == ' ') {
                    if (value_str.empty()) {
                        continue;
                    }
                    break;
                } else { // no digit
                    break;
                }
            }
            if (!value_str.empty()) {
                value = std::stoi(value_str);
                find = false;
            }
            auto path = topology.GetPath(id);
            if (path.empty()) {
                std::cout << "Error:" << id << ": Not found\n";
                continue;
            }
            std::vector<int> path_sons;
            for (auto it : path) {
                if (it.second == TO_SON) {
                    path_sons.push_back(it.first);
                }
            }
            auto next_id = path_sons.front();
            path_sons.erase(path_sons.begin());
            std::ostringstream msg_stream;
            msg_stream << "exec " << path_sons.size();
            for (auto it : path_sons) {
                msg_stream << " " << it;
            }     
            send_message(sockets.at(next_id), msg_stream.str());
            res = receive_message(sockets.at(next_id));
            std::cout << res << "\n";
        } else if (command == "pingall") {
            if (pids.empty()) {
                std::cout << "Error: no nodes\n";
                continue;
            }
            std::istringstream result(pre_result);
            std::set<int> responded_id;
            int temp;
            while (result >> temp) {
                responded_id.insert(temp);
            }
            std::set<int> all_id = topology.GetNodes();
            std::set<int> result_set;
            auto it_resp = responded_id.begin();
            auto it_all = all_id.begin();
            for (int i = 0; i < all_id.size(); i++) {
                if (*it_resp == *it_all) {
                    it_resp++;
                    it_all++;
                } else {
                    result_set.insert(*it_all);
                    it_all++;
                }
            }
            std::cout << "Ok: ";
            if (result_set.empty()) {
                std::cout << "-1\n";
                continue;
            }
            for (auto it : result_set) {
                std::cout << it << ";";
            }
            std::cout << "\n";
        } else if (command == "print") {
            std::cout << "---------------------------------\n";
            topology.Print();
            std::cout << "---------------------------------\n";
        } else if (command == "exit" || command == "quit" || command == "e" || command == "q") {
            for (auto it = sockets.begin(); it != sockets.end(); it++) {
                send_message(it->second, "kill");
                receive_message(it->second);
                kill(pids.at(it->first), SIGTERM);
            }
            break;
        } else {
            std::cout << "Error: incorrect input\n";
        }
    }

    return 0;
}