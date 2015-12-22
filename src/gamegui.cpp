/**
 * @author Michele Tomaiuolo - http://www.ce.unipr.it/people/tomamic
 * @license This software is free - http://www.gnu.org/licenses/gpl.html
 */

#include "gamegui.h"

#include <QGridLayout>
#include <QMessageBox>
#include <QPushButton>
#include <string>

GameGui::GameGui(Game* game)
{
    game_ = game;
    cols_ = game->cols();
    rows_ = game->rows();

    btn_state_stage.resize(rows_);

    auto grid = new QGridLayout;
    setLayout(grid);
    grid->setSpacing(0);

    for (auto y = 0; y < rows_; ++y) {
        btn_state_stage[y].resize(cols_);
        for (auto x = 0; x < cols_; ++x) {
            auto b = new QPushButton;
            auto w = 30, h = 30;
            if (x % 2 == 0) w = 15;
            if (y % 2 == 0) h = 15;
            b->setFixedSize(w, h);
            grid->addWidget(b, y, x);
            connect(b, &QPushButton::clicked,[=]{ handle_click(x, y); });

            CellButtonState state;
            state.btn = b;
            state.style = -1;
            state.val = 0;
            state.x = x;
            state.y = y;
            state.pending = true;
            btn_state_stage[y][x] = state;
        }
    }
    update_all_buttons();
}


void GameGui::update_button_state(CellButtonState &state) {
    auto val = game_->get_val(state.x, state.y);
    auto style = state.style;

    switch(val) {
    case CELL_JUNCTION:
        if (game_->count_neighbour_lines(state.x, state.y) > 0)
            style = STYLE_JUNCTION_ACTIVE;
        else {
            if ((game_->count_neighbour_crosses(state.x, state.y) > 0)
                    && (game_->count_neighbour_blanks(state.x, state. y) == 0))
                style = STYLE_JUNCTION_CLOSED;
            else
                style = STYLE_JUNCTION_DEFAULT;
        }
        break;

    case CELL_LINE:
        style = STYLE_LINE;
        break;

    case CELL_CROSS:
        style = STYLE_CROSS;
        break;

    case CELL_BLANK:
        /*        if ((state.x + state.y) % 2 == 0)
            style = -1;
        else*/
        style = STYLE_BLANK;
        break;

    default:
        // numero
        auto num = val - '0';
        if (game_->count_neighbour_lines(state.x, state.y) == num)
            style = STYLE_NUMBER_FILLED;
        else
            style = STYLE_NUMBER_DEFAULT;
        break;
    }

    state.pending = !(state.val == val && state.style == style);
    state.val = val;
    state.style = style;
}

void GameGui::update_button(CellButtonState &state)
{
    update_button_state(state);
    if (!state.pending) return;

    state.pending = false;
    auto b = state.btn;
    auto val = state.val;
    if (val == CELL_LINE) {
        val = (state.x % 2) ? '-' : '|';
    }

    b->setText(QString(1, val));
    if (state.style > -1){
        b->setVisible(true);
        b->setStyleSheet(QString::fromStdString(btn_styles[state.style]));
    }
    else
        b->setVisible(false);
}

void GameGui::update_game_state()
{
    update_all_buttons();

    if (game_->finished()) {
        QMessageBox::information(this,
                                 tr("Game finished"),
                                 tr(game_->message().c_str()));
        game_->remove_all_crosses();
        update_all_buttons();
        for (auto y = 0; y < rows_; y++) {
            for (auto x = 0; x < cols_; x++) {
                CellButtonState state = btn_state_stage[y][x];
                if (state.val == CELL_BLANK)
                    state.btn->setVisible(false);
                else
                    state.btn->setDisabled(true);
            }
        }
    }
}

void GameGui::update_all_buttons()
{
    for (auto y = 0; y < rows_; y++) {
        for (auto x = 0; x < cols_; x++) {
            update_button(btn_state_stage[y][x]);
        }
    }
}

void GameGui::handle_click(int x, int y)
{
    game_->play_at(x, y);
    update_game_state();
}
