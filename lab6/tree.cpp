#include <iostream>
#include <memory> // shared_ptr
#include <utility> // pair
#include "tree.hpp"

std::shared_ptr<TNode> TTree::create_node(int id) {
    return std::make_shared<TNode>(id, nullptr, nullptr);
}

std::shared_ptr<TNode> TTree::depth_search(std::shared_ptr<TNode> node, int id) {
    std::shared_ptr<TNode> result{nullptr};
    if (node != nullptr) {
        if (id == node->id_) {
            return node;
        }
        result = depth_search(node->son_, id);
        if (result == nullptr) {
            result = depth_search(node->brother_, id);
        }
    }

    return result;
}

void TTree::help_get_nodes(std::set<int> &s, std::shared_ptr<TNode> node) {
    if (node != nullptr) {
        s.insert(node->id_);
        help_get_nodes(s, node->son_);
        help_get_nodes(s, node->brother_);
    }
}

std::shared_ptr<TNode> TTree::breadth_search(std::shared_ptr<TNode> node, int id) {
    std::shared_ptr<TNode> result{nullptr};
    if (node != nullptr) {
        if (id == node->id_) {
            return node;
        }
        result = breadth_search(node->brother_, id);
        if (result == nullptr) {
            result = breadth_search(node->son_, id);
        }
    }

    return result;
}

TTree::pair_of_ptr TTree::remove_help(std::shared_ptr<TNode> iter_node, std::shared_ptr<TNode> prev_node, int id) {
    TTree::pair_of_ptr result = std::make_pair<std::shared_ptr<TNode>, std::shared_ptr<TNode>>(nullptr, nullptr);
    if (iter_node != nullptr) {
        if (id == iter_node->id_) {
            result.first = iter_node;
            result.second = prev_node;
            return result;
        }
        result = remove_help(iter_node->son_, iter_node, id);
        if (result.first == nullptr && result.second == nullptr) {
            result = remove_help(iter_node->brother_, iter_node, id);
        }
    }

    return result;
}

void TTree::print_help(std::shared_ptr<TNode> node, int right, int down) {
    if (node != nullptr) {
        for (int i = 0; i < down; i++) {
            std::cout << "\n";
        }
        for (int i = 0; i < right; i++) {
            std::cout << "  ";
        }
        std::cout << node->id_;
        print_help(node->son_, right + 1, 1);
        print_help(node->brother_, right, 1);
    }
}

bool TTree::help_get_path(std::shared_ptr<TNode> iter, std::vector<std::pair<int, Direction>> &path, int id) {
    if (iter == nullptr) {
        return false;
    }
    if (iter->id_ == id && iter->son_ == nullptr) {
        auto p = std::make_pair(iter->id_, TO_SON);
        path.push_back(p);
        return true;
    }
    if (iter->id_ == id && iter->son_ != nullptr) {
        auto p = std::make_pair(iter->id_, TO_SON);
        path.push_back(p);
        iter = iter->son_;
        p = std::make_pair(iter->id_, TO_BRO);
        path.push_back(p);
        while (iter->brother_ != nullptr) {
            iter = iter->brother_;
            auto p = std::make_pair(iter->id_, TO_BRO);
            path.push_back(p);
        }
        return true;
    }
    auto p = std::make_pair(iter->id_, TO_SON);
    path.push_back(p);
    std::shared_ptr<TNode> iter_son = iter;
    while (iter_son->son_ != nullptr) {
        iter_son = iter_son->son_;
        if (help_get_path(iter_son, path, id)) {
            return true;
        }
    }
    path.pop_back();
    p = std::make_pair(iter->id_, TO_BRO);
    path.push_back(p);
    std::shared_ptr<TNode> iter_bro = iter;
    while (iter_bro->brother_ != nullptr) {
        iter_bro = iter_bro->brother_;
        if (help_get_path(iter_bro, path, id)) {
            return true;
        }
    }
    path.pop_back();
    return false;
}

std::shared_ptr<TNode> TTree::Search(int id) {
    return depth_search(root_, id);
}

bool TTree::Insert(int id, int parent_id) {
    std::shared_ptr<TNode> new_node = create_node(id);
    std::shared_ptr<TNode> parent_node = depth_search(root_, parent_id);

    if (root_ == nullptr) {
        root_ = new_node;
        return true;
    }

    if (root_ != nullptr && parent_id == -1) {
        std::shared_ptr<TNode> it = root_;
        while (it->brother_ != nullptr) {
            it = it->brother_;
        }
        it->brother_ = new_node;
    }

    if (parent_node == nullptr) {
        return false;
    } 

    if (parent_node->son_ == nullptr) {
        parent_node->son_ = new_node;
        return true;
    }

    std::shared_ptr<TNode> iter_node = parent_node->son_;
    while (iter_node->brother_ != nullptr) {
        iter_node = iter_node->brother_;
    }
    iter_node->brother_ = new_node;

    return true;        
}

bool TTree::Remove(int id) {
    TTree::pair_of_ptr need_and_prev = remove_help(root_, nullptr, id);

    if (need_and_prev.first == nullptr && need_and_prev.second == nullptr) { // id not found
        return false;
    }

    if (need_and_prev.first == root_) {
        root_ = nullptr;
        return true;
    }

    if (need_and_prev.second->son_ == need_and_prev.first) { // если предыдущий является отцом
        need_and_prev.second->son_ = need_and_prev.first->brother_;
        need_and_prev.first = nullptr;
        return true;
    }

    if (need_and_prev.second->brother_ == need_and_prev.first) { // если предыдущий является братом
        need_and_prev.second->brother_ = need_and_prev.first->brother_;
        need_and_prev.first = nullptr;
        return true;
    }
}

std::vector<std::pair<int, Direction>> TTree::GetPath(int id) {
    std::vector<std::pair<int, Direction>> result;
    help_get_path(root_, result, id);
    return result;
}

std::set<int> TTree::GetNodes() {
    std::set<int> s;
    help_get_nodes(s, root_);
    return s;
}

void TTree::Print() {
    print_help(root_, 0, 0);
    std::cout << "\n";
}