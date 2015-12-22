#include <stdexcept>
#include <fstream>
#include "slitherlink.h"

using namespace std;


Slitherlink::Slitherlink()
{
}

Slitherlink::Slitherlink(string filename)
{
    load_from_file(filename);
}

void Slitherlink::play_at(int x, int y)
{
    auto val = get_val(x,y);


    switch(val) {
    case CELL_BLANK:
        if ((x + y) % 2 == 1)
            val = CELL_LINE;
        break;
    case CELL_LINE:
        val = CELL_CROSS;
        break;
    case CELL_CROSS:
        val = CELL_BLANK;
        break;
    case CELL_JUNCTION:
        autocomplete_junction(x, y);
        break;
    default:
        autocomplete_number(x, y);
        break;
    }
    set_val(x, y, val);
    commit_history();
}

bool Slitherlink::is_wrong() const
{
    for(int y = 0; y < rows_; y++) {
        for(int x = 0; x < cols_; x++) {
            auto val = get_val(x, y);

            switch(val) {
            case CELL_BLANK:
            case CELL_LINE:
            case CELL_CROSS:
                continue;
                break; // compiler complains
            }
            // junctions and numbers
            int lines_cnt = count_neighbours(x, y, CELL_LINE);
            int crosses_cnt = count_neighbours(x, y, CELL_CROSS);

            if (val == CELL_JUNCTION) {
                if (lines_cnt == 0 || lines_cnt == 2)
                    continue;
                // too many lines or 1 line and too many crosses
                if (lines_cnt == 3 || max_moves(x, y) - crosses_cnt <= 0)
                    return true;

                continue;
            }

            int num = val - '0';
            //it's a number cell, check for too many lines or too many crosses
            if (lines_cnt > num || crosses_cnt > (4 - num))
                return true;
        }
    }
    return false;
}


bool Slitherlink::finished() const
{
    // conversion from char to int
    // char val = '5';  /* just for example ... */
    // int num = val - '0';
    int total_lines = 0;

    for(int y = 0; y < rows_; y++) {
        for(int x = 0; x < cols_; x++) {
            auto val = get_val(x, y);
            if (val == CELL_LINE) {
                total_lines++;
            }

            // junctions
            if (val == CELL_JUNCTION) {
                int cnt = count_neighbour_lines(x, y);
                if (cnt != 0 && cnt != 2)
                    return false;
                continue;
            }
            // numbers
            int num = val - '0';
            if (0 <= num && num <= 3 && count_neighbour_lines(x, y) != num)
               return false;
        }
    }

    if (is_wrong())
        return false;

    for (auto y = 0; y < rows_; ++y) {
        for (auto x = 0; x < cols_; ++x) {
            if (board_[y][x] == CELL_LINE) {
                // a single loop would count all the lines in the board
                int lines = count_loop({x + 1, y}, {1, 0}, {x + 1, y}, 0);
                return lines == total_lines;
            }
        }
    }

    return true;
}

void Slitherlink::save_to_stream(ostream *os) {
    for(auto y = 0; y < rows_; y++) {
        for(auto x = 0; x < cols_; x++) {
            os->put(board_[y][x]);
        }
        os->put('\r');
    }
    os->flush();
}

bool Slitherlink::save_to_file(string filename) {
    ofstream file;
    file.open(filename);
    save_to_stream(&file);
    file.close();
    return true;
}

void Slitherlink::load_from_stream(istream *is) {
    vector<vector<char>> new_board;
    vector<char> row;
    char c;

    while (is->get(c)) {
        if (c == '\r' || c == '\n') {
            // ignore empty lines
            if (row.size() == 0) continue;

            new_board.push_back(row);
            row.clear();
            continue;
        }
        row.push_back(c);
    }
    board_ = new_board;
    rows_ = board_.size();
    cols_ = new_board[0].size();
}

bool Slitherlink::load_from_file(string filename) {
    ifstream file;
    file.open(filename);
    if (!file.is_open())
        return false;
    load_from_stream(&file);
    file.close();
    return true;
}


void Slitherlink::check_2s_on_corners(int x, int y) {
    // the special case for 2 in the board corners
    vector<complex<int>> corners = { { cols_ - 2, 1 },
                                     { 1, 1 },
                                     { 1, rows_ - 2 },
                                     { cols_ - 2, rows_ - 2}};

    //offsets = { {1, -1}, {-1, -1}, {-1, 1}, {1, 1} }
    for(auto i = 0; i < 4; i++) {
        complex<int> pos = corners[i];
        if (x != pos.real() || y != pos.imag())
            continue;

        complex<int> offset = DIAGONAL_DIRS[i];
        set_val(pos.real() + offset.imag() * 2, pos.imag() + offset.imag(), CELL_LINE);
        set_val(pos.real() + offset.real(), pos.imag() + offset.real() * 2, CELL_LINE);
        break;
    }
}

// chamar somente para 1's e 3's...
void Slitherlink::check_corner(int x, int y, char fill_with) {
    for(auto dir: DIAGONAL_DIRS) {
        vector<complex<int>> coords = {
            { x + dir.real() * 2, y + dir.imag() },
            { x + dir.real(), y + dir.imag() * 2 }
        };
        bool is = true;
        for(auto cell_pos: coords) {
            if (is_on_bounds(cell_pos.real(), cell_pos.imag())
              && get_val(cell_pos.real(), cell_pos.imag()) != CELL_CROSS) {
                is = false;
                break;
            }
        }
        if (!is)
            continue;

        // is a corner, fill the neighbour cells
        set_val(x + dir.real(), y, fill_with);
        set_val(x, y + dir.imag(), fill_with);
        break;
    }
}

void Slitherlink::play_like_a_human(int x, int y) {
    char val = get_val(x, y);
    int num = val - '0';

    switch(num) {
    case 1:
        check_corner(x, y, CELL_CROSS);
        break;

    case 2:
        check_2s_on_corners(x, y);
        break;

    case 3:
        check_corner(x, y, CELL_LINE);

        for(auto i = 0; i < 4; i++) {
            complex<int> pos = {x, y};

            // add vert/horz lines if this 3 has another 3 as a neighbour
            complex<int> coord = ORTHOGONAL_DIRS[i];
            complex<int> pos1 = pos + coord * 2;

            if (is_on_bounds(pos1.real(), pos1.imag()) && get_val(pos1.real(), pos1.imag()) == '3') {
                set_val(x + coord.real(), y + coord.imag(), CELL_LINE);
                set_val(x - coord.real(), y - coord.imag(), CELL_LINE);
            }

            // check for 3s in the diagonals
            coord = DIAGONAL_DIRS[i];
            pos1 = pos + coord * 2;
            if (is_on_bounds(pos1.real(), pos1.imag()) && get_val(pos1.real(), pos1.imag()) == '3') {
                set_val(x, y - coord.imag(), CELL_LINE);
                set_val(x - coord.real(), y, CELL_LINE);
            }
        }
        break;
    }
}

bool Slitherlink::autocomplete_number(int x, int y) {
    char val = board_[y][x];
    int num = val - '0';

    if (!(0 <= num && num <= 3))
        return false;

    int lines_cnt = count_neighbour_lines(x, y);
    char fill = 0;

    if (lines_cnt == num) {
        fill = CELL_CROSS;
    }
    else {
        int blocks_cnt = count_neighbours(x, y, CELL_CROSS);
        int comp = int(4) - num;
        if(blocks_cnt == comp) {
            fill = CELL_LINE;
        }
    }
    return fill && fill_blank_neighbours(x, y, fill);
}

bool Slitherlink::autocomplete_junction(int x, int y) {
    char val = get_val(x, y);

    if (val != CELL_JUNCTION)
        return false;

    char fill = 0;
    int lines_cnt = count_neighbours(x, y, CELL_LINE);
    // ok
    if (lines_cnt == 2)
        fill = CELL_CROSS;
    // only 1 way to go
    else if (lines_cnt == 1 && count_neighbours(x, y, CELL_BLANK) == 1)
        fill = CELL_LINE;
    // place crosses on "dead ends"
    else {
        // the case for the "dead end"
        int blocks_cnt = count_neighbours(x, y, CELL_CROSS);
        if ((max_moves(x, y) - blocks_cnt) == 1)
            fill = CELL_CROSS;
    }
    return fill && fill_blank_neighbours(x, y, fill);
}

bool Slitherlink::autocomplete(bool all) {
    unsigned int change_count = 0;

    do {
        change_count = current_hist_item.size();

        for(auto y = 0; y < rows_; y++) {
            for(auto x = 0; x < cols_; x++) {
                autocomplete_number(x, y);
                autocomplete_junction(x, y);
                play_like_a_human(x, y);

                if (current_hist_item.size() > 0 && !all) {
                    return commit_history();
                }
            }
        }
    } while (all && change_count != current_hist_item.size());
    build_segments();
    return commit_history();
}

void Slitherlink::remove_all_crosses()
{
    for(int y = 0; y < rows_; y++) {
        for(int x = 0; x < cols_; x++) {
            auto val = get_val(x, y);
            if (val == CELL_CROSS) {
                set_val(x, y, CELL_BLANK);
            }
        }
    }
}

bool Slitherlink::is_on_bounds(int x, int y) const {
    return (0 <= x) && (x < cols_) && (0 <= y) && (y < rows_);
}

bool Slitherlink::fill_blank_neighbours(int x, int y, char val) {
    bool result = false;

    for(auto dir:ORTHOGONAL_DIRS) {
        int x1 = x + dir.real();
        int y1 = y + dir.imag();

        if (!is_on_bounds(x1, y1))
            continue;

        if (board_[y1][x1] == CELL_BLANK)
        {
            result = true;
            set_val(x1, y1, val);
        }
    }

    return result;
}

int Slitherlink::max_moves(int x, int y) const
{
    int moves = 4;
    if (y == 0 || y == rows_ - 1)
        moves--
               ;
    if (x == 0 || x == cols_ -1)
        moves--;

    return moves;
}

char Slitherlink::get_val(int x, int y) const
{
    return board_[y][x];
}

char Slitherlink::set_val(int x, int y, char val) {
    if (!is_on_bounds(x, y)) {
        x = 0;
        y = 0;
    }

    auto c = board_[y][x];
    if (val != c) {

        board_[y][x] = val;

        cellstate state = { x = x,
                            y = y,
                            val = c };
        current_hist_item.push_back(state);
    }
    return c;
}

bool Slitherlink::commit_history() {
    if (!current_hist_item.size())
        return false;

        history_list.push_back(current_hist_item);
        current_hist_item.clear();
        return true;
}

bool Slitherlink::get_line_dir(int x, int y, complex<int> &dir) const {
    int count = 0;

    for(auto d: ORTHOGONAL_DIRS) {
        int x1 = x + d.real();
        int y1 = y + d.imag();

        if (!is_on_bounds(x1, y1) || get_val(x1, y1) != CELL_LINE)
            continue;

        if (count == 0)
            dir = d;

        count++;
    }

    return count == 1;
}

bool Slitherlink::find_endpoint(complex<int> pos, complex<int> stop, complex<int> dir, complex<int> &end, int &lines) const
{
    for (auto move : ORTHOGONAL_DIRS) {
        if (move == -dir) // don't go backwards
            continue;

        auto new_pos = pos + move;
        if (!is_on_bounds(new_pos.real(), new_pos.imag())
                || get_val(new_pos.real(), new_pos.imag()) != CELL_LINE)
            continue;

        end = new_pos + move;
        lines++;
        if (end == stop)
            return true;

        return find_endpoint(end, stop, move, end, lines);
        // tail recursion
    }
    return lines > 0;
}

complex<int> Slitherlink::get_endpoint(complex<int> coord) const {
    for(auto seg: line_segments) {
        if (seg.begin == coord) return seg.end;
        if (seg.end == coord) return seg.begin;
    }
    return NO_COORD;
}

void Slitherlink::build_segments()
{
    line_segments.clear();
    for(int y = 0; y < rows_; y++) {
        for(int x = 0; x < cols_; x++) {
            auto val = get_val(x, y);
            complex<int> dir;
            if (val != CELL_JUNCTION
                    || !get_line_dir(x, y, dir)
                    || get_endpoint(dir) != NO_COORD) {
                continue;
            }

            line_segment seg;
            seg.begin = {x, y};
            if (!find_endpoint(seg.begin, seg.begin, dir, seg.end, seg.length))
                continue;

            line_segments.push_back(seg);
        }
    }
}

void Slitherlink::undo() {
    if (!history_list.size())
        return;

    auto hist = history_list[history_list.size() - 1];
    history_list.pop_back();
    for(auto cs: hist) {
        board_[cs.y][cs.x] = cs.val;
    }
}

int Slitherlink::count_neighbours(int x, int y, char test_val) const {
    int count = 0;

    for(auto dir:ORTHOGONAL_DIRS) {
        int x1 = x + dir.real();
        int y1 = y + dir.imag();

        if (!is_on_bounds(x1, y1) || get_val(x1, y1) != test_val)
            continue;

        count++;
    }

    return count;
}

int Slitherlink::count_neighbour_lines(int x, int y) const {
    return count_neighbours(x, y, CELL_LINE);
}

int Slitherlink::count_neighbour_crosses(int x, int y) const {
    return count_neighbours(x, y, CELL_CROSS);
}

int Slitherlink::count_neighbour_blanks(int x, int y) const {
    return count_neighbours(x, y, CELL_BLANK);
}

int Slitherlink::count_loop(complex<int> pos, complex<int> dir, complex<int> stop, int lines) const
{
    if (pos == stop && lines > 0) return lines;

    // TODO: pay attention to crossroads and branches!
    // is_wrong() negatively checks for branches and crossroads
    for (auto move : ORTHOGONAL_DIRS) {
        if (move == -dir) // don't go backwards
            continue;

        auto new_pos = pos + move;
        if (!is_on_bounds(new_pos.real(), new_pos.imag())
                || get_val(new_pos.real(), new_pos.imag()) != CELL_LINE)
            continue;

        return count_loop(new_pos + move, move, stop, lines + 1);
        // tail recursion
    }
    return 0;  // this way, open paths may be considered
}
