#include <QtWidgets>

#include "mainwindow.h"
#include "slitherlink.h"
#include "gamegui.h"

MainWindow::MainWindow()
{
    create_actions();
    create_menu();
    create_toolBar();
    create_statusbar();
    set_current_game(new Slitherlink("../game_18x10.slgame"));
}

void MainWindow::set_current_game(Game *new_game) {
    // delete the current game, if exists
    delete game;
    delete game_gui;

    game = new_game;
    game_gui = new GameGui(game);
    setCentralWidget(dynamic_cast<QWidget*>(game_gui));
    adjustSize();
}

void MainWindow::open()
{
    QString filename = QFileDialog::getOpenFileName(this, NULL, NULL, "*.slgame");
    if (filename.isEmpty())
        return;

    auto *game = new Slitherlink();
    if (game->load_from_file(filename.toStdString()))
    {
        set_current_game(game);
    }
}

bool MainWindow::save()
{
    auto filename = QFileDialog::getSaveFileName(this, "Save game", NULL, "*.slgame");
    if (filename.isEmpty())
        return false;

    if (game->save_to_file(filename.toStdString()))
    {
        statusBar()->showMessage(tr("File saved"), 3000);
        return true;
    }
    statusBar()->showMessage(tr("Error loading file"), 3000);
    return false;
}

void MainWindow::create_actions()
{
    open_act = new QAction("L&oad saved game...", this);
    open_act->setShortcuts(QKeySequence::Open);
    open_act->setStatusTip("Load a saved game");
    connect(open_act, SIGNAL(triggered()), this, SLOT(open()));

    save_act = new QAction(tr("&Save..."), this);
    save_act->setShortcuts(QKeySequence::Save);
    save_act->setStatusTip("Save the current game");
    connect(save_act, SIGNAL(triggered()), this, SLOT(save()));

    exit_act = new QAction(tr("E&xit"), this);
    exit_act->setShortcuts(QKeySequence::Quit);
    exit_act->setStatusTip(tr("Exit the game"));
    connect(exit_act, SIGNAL(triggered()), this, SLOT(close()));

    //---

    autocomplete_act = new QAction("Autocomplete", this);
    autocomplete_act->setStatusTip("Autocomplete like a human where possible");
    connect(autocomplete_act, SIGNAL(triggered()), this, SLOT(autocomplete()));

    autocomplete_all_act = new QAction("Autocomplete &all", this);
    autocomplete_all_act->setStatusTip("Autocomplete all cells like a human where possible");
    connect(autocomplete_all_act, SIGNAL(triggered()), this, SLOT(autocomplete_all()));

    remove_crosses_act = new QAction("Remove all crosses", this);
    remove_crosses_act->setStatusTip("Remove all crosses from the board");
    connect(remove_crosses_act, SIGNAL(triggered()), this, SLOT(remove_crosses()));

    undo_act = new QAction("Undo", this);
    undo_act ->setShortcut(QKeySequence::Undo);
    undo_act ->setStatusTip("Remove all crosses from the board");
    connect(undo_act , SIGNAL(triggered()), this, SLOT(undo()));

}

void MainWindow::create_menu()
{
    file_menu = menuBar()->addMenu("&File");
    file_menu->addAction(open_act);
    file_menu->addAction(save_act);
    file_menu->addSeparator();
    file_menu->addAction(exit_act);

    game_menu = menuBar()->addMenu("&Game");
    game_menu->addAction(undo_act);
    game_menu->addSeparator();
    game_menu->addAction(remove_crosses_act);
    game_menu->addAction(autocomplete_act);
    game_menu->addAction(autocomplete_all_act);

    menuBar()->addSeparator();
}

void MainWindow::create_toolBar()
{
    helpme_tb = addToolBar("Help me");
    helpme_tb->addAction(autocomplete_act);
    helpme_tb->addAction(autocomplete_all_act);
    helpme_tb->addAction(undo_act);
}

void MainWindow::create_statusbar()
{
    statusBar()->showMessage("Ready");
}

void MainWindow::autocomplete() {
    game->autocomplete(false);
    game_gui->update_game_state();
}

void MainWindow::autocomplete_all() {
    game->autocomplete(true);
    game_gui->update_game_state();
}


void MainWindow::remove_crosses() {
    game->remove_all_crosses();
    game_gui->update_game_state();
}

void MainWindow::undo() {
    game->undo();
    game_gui->update_game_state();
}


