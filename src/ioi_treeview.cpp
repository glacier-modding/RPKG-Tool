#include "global.h"
#include "ioi_treeview.h"
#include <iostream>

IOITreeView::IOITreeView() = default;

void IOITreeView::InitializeTreeView() {
    Reset();

    uint32_t hash_count = 0;
    uint32_t hash_count_current = 0;

    for (auto& rpkg : rpkgs)
        hash_count += rpkg.hash.size();

    for (auto& rpkg : rpkgs) {
        for (auto& hash : rpkg.hash) {
            hash_count_current++;
            percent_progress = (100 * hash_count_current) / hash_count;

            std::unordered_map<uint64_t, uint64_t>::iterator it = hash_list_hash_map.find(hash.hash_value);

            if (it == hash_list_hash_map.end())
                continue;
            
            auto& hash_list_string = hash_list_hash_strings.at(it->second);

            std::string_view ioi_path(hash_list_string.c_str(), hash_list_string.length());

            size_t pos = 0;

            if (ioi_path.substr(0, 2) == "[a") {
                size_t bracket = ioi_path.find_first_of(']');

                if (bracket != std::string::npos) {
                    ioi_path = ioi_path.substr(0, bracket);
                    size_t start = 11;
                    size_t end = ioi_path.find_last_of('/') - 10;
                    std::string parent = "";

                    pos = ioi_path.substr(start, end).find_first_of('/');

                    while (pos != std::string::npos) {
                        std::string path(ioi_path.substr(start, pos));

                        if (node_map.find(path) != node_map.end())
                            parent = path;
                        else {
                            node_map[path].id = node_map.size();

                            if (parent == "") {
                                parent = path;
                                node_map[path].name = path;
                                root_nodes.insert(path);
                            }
                            else {
                                node_map[path].parent = parent;
                                node_map[parent].children.insert(path);
                                parent = path;
                                node_map[path].name = path.substr(path.find_last_of('/') + 1);
                            }
                        }

                        pos = ioi_path.substr(start, end).find('/', pos + 1);
                    }

                    pos = ioi_path.find_last_of('/') + 1;

                    node_map[std::string(ioi_path.substr(start, end - 1))].files[std::string(ioi_path.substr(pos))] = hash.hash_value;
                }
            }
        }
    }

    initialized = true;
}

void IOITreeView::DisplayTreeView() {
    for (auto& root_node : root_nodes) {
        DisplayTreeNodes(root_node, 0);
    }

    /*for (auto& root_node : root_nodes)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

        if (node_map[root_node].selected)
            flags |= ImGuiTreeNodeFlags_Selected;

        bool open = ImGui::TreeNodeEx(root_node.c_str(), flags);

        if (ImGui::IsItemClicked())
        {
            std::cout << root_node << " selected!" << std::endl;

            if (selected_node != "")
                node_map[selected_node].selected = false;

            node_map[root_node].selected = true;
            selected_node = root_node;
        }

        if (open)
        {
            for (auto& node : node_map[root_node].children)
            {
                DisplayTreeNodes(node);
            }

            ImGui::TreePop();
        }
    }*/
}

void IOITreeView::DisplayTreeNodes(const std::string& root_node, uint32_t levels) {
    if (levels == 0) {
        std::cout << root_node << std::endl;
    } else {
        std::string spacing = "";
        for (uint32_t i = 0; i < levels - 1; i++) {
            spacing += "|   ";
        }
        std::cout << spacing << "|-->" << node_map[root_node].name << std::endl;
    }

    if (node_map[root_node].children.size() > 0) {
        for (auto& node : node_map[root_node].children) {
            DisplayTreeNodes(node, levels + 1);
        }
    }
    else
    {
        levels++;

        for (auto& file : node_map[root_node].files) {
            std::string spacing = "";
            for (uint32_t i = 0; i < levels - 1; i++) {
                spacing += "|   ";
            }
            std::cout << spacing << "|-->" << file.first << std::endl;
        }
    }

    /*if (node_map[root_node].children.size() > 0)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

        if (node_map[root_node].selected)
            flags |= ImGuiTreeNodeFlags_Selected;

        bool open = ImGui::TreeNodeEx(node_map[root_node].name.c_str(), flags);

        if (ImGui::IsItemClicked())
        {
            std::cout << root_node << " selected!" << std::endl;

            if (selected_node != "")
                node_map[selected_node].selected = false;

            node_map[root_node].selected = true;
            selected_node = root_node;
        }

        if (open)
        {
            for (auto& node : node_map[root_node].children)
            {
                DisplayTreeNodes(node);
            }

            ImGui::TreePop();
        }
    }
    else
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        if (node_map[root_node].selected)
            flags |= ImGuiTreeNodeFlags_Selected;

        bool open = ImGui::TreeNodeEx(node_map[root_node].name.c_str(), flags);

        if (ImGui::IsItemClicked())
        {
            std::cout << root_node << " selected!" << std::endl;

            if (selected_node != "")
                node_map[selected_node].selected = false;

            node_map[root_node].selected = true;
            selected_node = root_node;
        }
    }*/
}

bool IOITreeView::IsInitialized() {
    return initialized;
}

void IOITreeView::Reset() {
    initialized = false;
    selected_node = "";
    std::unordered_map<std::string, IOITreeNode>().swap(node_map);
    std::unordered_set<std::string>().swap(root_nodes);
}