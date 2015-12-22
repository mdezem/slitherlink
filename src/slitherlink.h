#ifndef SLITHERLINK_H
#define SLITHERLINK_H

#include "game.h"
#include <vector>
#include <complex>

using namespace std;

struct cellstate {
    int x;
    int y;
    char val;
};

struct line_segment {
    complex<int> begin;
    complex<int> end;
    int length;
};

class Slitherlink: public Game
{
public:
    Slitherlink();
    Slitherlink(string filename);
    virtual void play_at(int x, int y);
    virtual int cols() const {return cols_; }
    virtual int rows() const { return rows_; }
    virtual char get_val(int x, int y) const;
    virtual bool finished() const;
    virtual string message() const { return "Puzzle solved"; }

    virtual bool autocomplete(bool all);
    virtual void remove_all_crosses();
    virtual int count_neighbour_lines(int x, int y) const;
    virtual int count_neighbour_blanks(int x, int y) const;
    virtual int count_neighbour_crosses(int x, int y) const;
    virtual void undo();

    virtual void load_from_stream(istream *is);
    virtual void save_to_stream(ostream *os);

    virtual bool save_to_file(string filename);
    virtual bool load_from_file(string filename);
private:
    // helpers
    int count_neighbours(int x, int y, char test_val) const;
    bool fill_blank_neighbours(int x, int y, char val);
    int max_moves(int x, int y) const;
    bool is_on_bounds(int x, int y) const;
    char set_val(int x, int y, char val);
    bool is_wrong() const;
    int count_loop(complex<int> pos, complex<int> dir, complex<int> stop, int lines) const;
    void check_2s_on_corners(int x, int y);

    // basic autocomplete
    bool autocomplete_number(int x, int y);
    bool autocomplete_junction(int x, int y);

    // "play like a human"
    void play_like_a_human(int x, int y);
    void check_corner(int x, int y, char fill_with);

    // history
    vector<vector<cellstate>> history_list;
    vector<cellstate> current_hist_item;
    bool commit_history();

    vector<line_segment> line_segments;
    void build_segments();
    complex<int> get_endpoint(complex<int> coord) const;

    complex<int> NO_COORD = { -1, -1 };

    // default game
    int cols_ = 11;
    int rows_ = 11;
    vector<vector<char>> board_ = {
        {'+', ' ', '+', ' ', '+', ' ', '+', ' ', '+', ' ', '+'},
        {' ', ' ', ' ', '3', ' ', '2', ' ', '2', ' ', ' ', ' '},
        {'+', ' ', '+', ' ', '+', ' ', '+', ' ', '+', ' ', '+'},
        {' ', ' ', ' ', '0', ' ', ' ', ' ', ' ', ' ', '2', ' '},
        {'+', ' ', '+', ' ', '+', ' ', '+', ' ', '+', ' ', '+'},
        {' ', ' ', ' ', '2', ' ', ' ', ' ', ' ', ' ', '1', ' '},
        {'+', ' ', '+', ' ', '+', ' ', '+', ' ', '+', ' ', '+'},
        {' ', ' ', ' ', '0', ' ', ' ', ' ', ' ', ' ', '2', ' '},
        {'+', ' ', '+', ' ', '+', ' ', '+', ' ', '+', ' ', '+'},
        {' ', ' ', ' ', '2', ' ', ' ', ' ', '2', ' ', ' ', ' '},
        {'+', ' ', '+', ' ', '+', ' ', '+', ' ', '+', ' ', '+'} };

    // direction vectors
    vector<complex<int>> ORTHOGONAL_DIRS = { {0, -1}, {-1, 0}, {0, 1}, {1, 0} };
    vector<complex<int>> DIAGONAL_DIRS = { {1, -1}, {-1, -1}, {-1, 1}, {1, 1} };
    bool get_line_dir(int x, int y, complex<int> &dir) const;
    bool find_endpoint(complex<int> pos, complex<int> stop, complex<int> dir, complex<int> &end, int &lines) const;
};

#endif // SLITHERLINK_H
