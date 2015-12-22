#ifndef GAME_H
#define GAME_H

#include <string>
#include <istream>
#include <ostream>

#define CELL_BLANK    ' '
#define CELL_LINE     '*'
#define CELL_CROSS    'x'
#define CELL_JUNCTION '+'

using namespace std;

class Game
{
public:
    virtual void play_at(int x, int y) = 0;
    virtual int cols() const = 0;
    virtual int rows() const = 0;
    virtual char get_val(int x, int y) const = 0;
    virtual bool finished() const = 0;
    virtual string message() const = 0;

    virtual int count_neighbour_lines(int x, int y) const = 0;
    virtual int count_neighbour_crosses(int x, int y) const = 0;
    virtual int count_neighbour_blanks(int x, int y) const = 0;
    virtual void undo() = 0;

    virtual bool autocomplete(bool all) = 0;
    virtual void remove_all_crosses() = 0;

    virtual void load_from_stream(istream *is) = 0;
    virtual void save_to_stream(ostream *os) = 0;

    virtual bool save_to_file(string filename) = 0;
    virtual bool load_from_file(string filename) = 0;

    virtual ~Game() {}
};

#endif // GAME_H
