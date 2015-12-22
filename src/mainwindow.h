#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "game.h"
#include "gamegui.h"


class QAction;
class QMenu;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    void set_current_game(Game *new_game);

private slots:
    void open();
    bool save();
    void autocomplete();
    void autocomplete_all();
    void remove_crosses();
    void undo();

private:
    void create_actions();
    void create_menu();
    void create_toolBar();
    void create_statusbar();

    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);

    Game *game = NULL;
    GameGui *game_gui = NULL;

    QMenu *file_menu;
    QMenu *game_menu;
    QToolBar *helpme_tb;
    QAction *open_act;
    QAction *save_act;
    QAction *exit_act;
    QAction *autocomplete_act;
    QAction *autocomplete_all_act;
    QAction *remove_crosses_act;
    QAction *undo_act;
};

#endif // MAINWINDOW_H

