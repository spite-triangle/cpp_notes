#include <QApplication>

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

#include "Mainwindow.h"
#include "DataTreeViewWidget.h"


TEST_CASE("TreeView"){
    DataTreeViewWidget* tree = new DataTreeViewWidget;
    tree->init();

    tree->appendRootItem("test");


    tree->show();


    DataTreeView::connect(tree, &DataTreeViewWidget::sig_clicked, tree, [](const QModelIndex &curr, std::shared_ptr<DataTreeView> pTree){
        pTree->appendChildren(curr, "ch1");
        pTree->appendChildren(curr, "ch2");
        pTree->appendChildren(curr, "ch3");

        pTree->expand(curr);
    } );

}

TEST_CASE("MainWindow"){
    Mainwindow* m = new Mainwindow;
    m->show();
}

int main(int argc, char  *argv[])
{
    doctest::Context context;

    context.addFilter("test-case", "MainWindow");
    context.applyCommandLine(argc, argv);

    QApplication a(argc, argv); 


    context.run();

    return a.exec();
}
