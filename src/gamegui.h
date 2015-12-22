/**
 * @author Michele Tomaiuolo - http://www.ce.unipr.it/people/tomamic
 * @license This software is free - http://www.gnu.org/licenses/gpl.html
 */

#ifndef GAMEGUI_H
#define GAMEGUI_H

#include "game.h"

#include <vector>
#include <QWidget>
#include <QPushButton>

#define STYLE_JUNCTION_ACTIVE  0
#define STYLE_JUNCTION_CLOSED  1
#define STYLE_JUNCTION_DEFAULT 2
#define STYLE_LINE             3
#define STYLE_CROSS            4
#define STYLE_BLANK            5
#define STYLE_NUMBER_DEFAULT   6
#define STYLE_NUMBER_FILLED    7

using namespace std;

struct CellButtonState {
    QPushButton *btn;
    int x;
    int y;
    char val;
    int style;
    bool pending;
};

class GameGui : public QWidget
{
    Q_OBJECT

public:
    GameGui(Game* game);
    void handle_click(int x, int y);
    void update_game_state();

private:
    void update_all_buttons();
    void update_button_state(CellButtonState &state);
    void update_button(CellButtonState &state);

    Game* game_;
    int cols_;
    int rows_;
    vector<vector<CellButtonState>> btn_state_stage;

    vector<string> btn_styles = {
        // STYLE_JUNCTION_ACTIVE
        "background: #55f; color: #55a; font-size: 13px; font-weight: bold;",

        // STYLE_JUNCTION_CLOSED
        "background: silver; color: gray; font-size: 13px; font-weight: bold;",

        // STYLE_JUNCTION_DEFAULT
        "background: white; color: #333; font-size: 13px; font-weight: bold;",

        // STYLE_LINE
        "background: #55f; color: blue;",

        // STYLE_BLOCK
        "background: silver; color: gray; font-size: 11px; font-weight: bold;",

        // STYLE_BLANK
        "background: white;",

        // STYLE_NUMBER_DEFAULT
        "background: white; color: green; font-size: 13px; font-weight: bold;",

        // STYLE_NUMBER_FILLED
        "background: #afa; color: green; font-size: 13px; font-weight: bold;"
    };
};

#endif // GAMEGUI_H
