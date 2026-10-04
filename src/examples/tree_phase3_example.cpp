#include "CASIO_GUI/casio.hpp"

int main()
{
    casio::GUI gui;

    auto* tree = new casio::Tree(
        {20, 25, 250, 165},
        18,
        14,
        true,
        true,
        2
    );

    int root = tree->addRoot(
        "PremiereApp",
        true
    );

    int src = tree->addNode(
        root,
        "src",
        true
    );

    tree->addNode(src, "main.cpp");

    int guiNode = tree->addNode(
        src,
        "CASIO_GUI",
        true
    );

    int itemNode = tree->addNode(
        guiNode,
        "item",
        true
    );

    tree->addNode(itemNode, "item.hpp");
    tree->addNode(itemNode, "item.cpp");

    int storageNode = tree->addNode(
        guiNode,
        "storage",
        false
    );

    tree->addNode(
        storageNode,
        "storage.hpp"
    );

    tree->setOnSelected(
        [](int id, const std::string& label)
        {
            (void)id;
            (void)label;
        }
    );

    tree->setOnActivated(
        [](int id, const std::string& label)
        {
            (void)id;
            (void)label;
        }
    );

    gui.item.addItem(tree);
    gui.runGUI();

    return 1;
}
