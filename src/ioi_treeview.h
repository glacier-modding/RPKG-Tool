#pragma once
#include <unordered_map>
#include <string>
#include <unordered_set>

class IOITreeView
{
public:
    struct IOITreeNode
    {
        size_t id = 0;
        std::string name = "";
        std::string parent = "";
        std::unordered_set<std::string> children;
        std::unordered_map<std::string, uint64_t> files;
        bool selected = false;
        bool expanded = false;
    };

    IOITreeView();

    void InitializeTreeView();
    void DisplayTreeView();
    void DisplayTreeNodes(const std::string& root_node, uint32_t levels);
    bool IsInitialized();
    void Reset();

    bool initialized = false;
    std::string selected_node = "";
    std::unordered_map<std::string, IOITreeNode> node_map;
    std::unordered_set<std::string> root_nodes;
};