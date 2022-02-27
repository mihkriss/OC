#ifndef TREE_HPP
#define TREE_HPP 1

#include <memory> // shared_ptr
#include <utility> // pair
#include <vector>
#include <set>

enum Direction {
    TO_SON, TO_BRO
};

struct TNode {
    int id_{0};
    std::shared_ptr<TNode> son_{nullptr};
    std::shared_ptr<TNode> brother_{nullptr};

    TNode(int id, const std::shared_ptr<TNode> &son, const std::shared_ptr<TNode> &brother) : id_{id}, son_{son}, brother_{brother} {}
    TNode(std::shared_ptr<TNode> node) : id_{node->id_}, son_{node->son_}, brother_{node->brother_} {}
};

class TTree {
public:
    using pair_of_ptr = std::pair<std::shared_ptr<TNode>, std::shared_ptr<TNode>>;
private:
    std::shared_ptr<TNode> root_{nullptr};
    std::shared_ptr<TNode> create_node(int id);
    std::shared_ptr<TNode> depth_search(std::shared_ptr<TNode> node, int id);
    void help_get_nodes(std::set<int> &s, std::shared_ptr<TNode> node);
    std::shared_ptr<TNode> breadth_search(std::shared_ptr<TNode> node, int id);
    pair_of_ptr remove_help(std::shared_ptr<TNode> iter_node, std::shared_ptr<TNode> prev_node, int id);
    void print_help(std::shared_ptr<TNode> node, int right, int down);
    bool help_get_path(std::shared_ptr<TNode> node, std::vector<std::pair<int, Direction>> &path, int id);
public:
    TTree() = default;
    TTree(const TTree &tree) : root_{tree.root_} {}
    TTree(TTree &&tree) : root_{std::move(tree.root_)} {}
    std::shared_ptr<TNode> Search(int id);
    bool Insert(int id, int parent_id);
    bool Remove(int id);
    std::vector<std::pair<int, Direction>> GetPath(int id);
    std::set<int> GetNodes();
    void Print();
};

#endif // TREE_HPP