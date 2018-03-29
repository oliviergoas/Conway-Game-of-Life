#include <iostream>
#include <vector>
#include <thread>
#include <ncurses.h>

using namespace std;

namespace {
	typedef vector<vector<bool>> Matrix;

    int start_ncurses () {
        initscr(); // starts ncurses

        if (! has_colors()) {
            endwin ();
            cerr << "Your terminal does not support color" << endl;
            return 1;
        }
        start_color (); // Starts color
        init_pair(1, COLOR_WHITE, COLOR_WHITE);

        cbreak();
        noecho();
        return 0;
    } // start_ncurses()

    string getString() {
        string input;
        nocbreak();
        echo();

        int ch = getch();
        while (ch != '\n') {
            input.push_back(ch);
            ch = getch();
        }

        cbreak();
        noecho();
        return input;
    }
	
	Matrix init(const unsigned size) {
		Matrix matrix(size);
		for (unsigned i = 0; i < size; ++i)
			matrix[i].resize(size);
		for (unsigned i = 0; i < size; ++i) {
			for (unsigned j = 0; j < matrix[0].size(); ++j)
				matrix[i][j] = false;
		}
		return matrix;
	} // init()

    void show(const Matrix &matrix) {
        for(unsigned i = 0; i < matrix.size(); ++i) {
            for(unsigned j = 0; j < matrix[i].size(); ++j) {
                if(matrix[i][j]) {
                    attron(COLOR_PAIR(1));
                    printw("  ");
                    attroff(COLOR_PAIR(1));
                }
                else {
                    printw("  ");
                }
            }
            addch('\n');
        }
    } // show()

	void copy(Matrix &oldMatrix, Matrix &newMatrix) {
		for (unsigned i = 0; i < oldMatrix.size(); ++i) {
			for (unsigned j = 0; j < oldMatrix[0].size(); ++j)
				newMatrix[i][j] = oldMatrix[i][j];
		}
	} // copy()
    
	void setAutomaton(Matrix &matrix, const string &automaton) {
		if (automaton == "glider") {
			matrix[0][0] = true;
			matrix[2][0] = true;
			matrix[2][1] = true;
			matrix[1][2] = true;
			matrix[1][1] = true;
		}
		else if (automaton == "tetris") {
			matrix[4][4] = true;
			matrix[5][3] = true;
			matrix[5][4] = true;
			matrix[5][5] = true;
		}
	} // setAutomaton()

	unsigned countNeighbors(const Matrix &matrix, const unsigned i, const unsigned j) {
		unsigned value = 0, last = matrix.size()-1;
		
		if (i < last             && matrix[i+1][j])   ++value;
		if (i < last && j > 0    && matrix[i+1][j-1]) ++value;
		if (            j > 0    && matrix[i]  [j-1]) ++value;
		if (i > 0    && j > 0    && matrix[i-1][j-1]) ++value;
		if (i > 0                && matrix[i-1][j])   ++value;
		if (i > 0    && j < last && matrix[i-1][j+1]) ++value;
		if (            j < last && matrix[i]  [j+1]) ++value;
		if (i < last && j < last && matrix[i+1][j+1]) ++value;
		
		return value;
	}

	bool evolve(const Matrix &matrix, const unsigned i, const unsigned j) {
		unsigned neighbors = countNeighbors(matrix, i, j);
		if (matrix[i][j]) {
			if (neighbors < 2 || neighbors > 3)	return false;
		}
		else
			if (neighbors == 3) return true;
		return matrix[i][j];
	}

	void nextGeneration(Matrix &matrix) {
		Matrix matrixTmp = init(matrix.size());
		copy(matrix, matrixTmp);
		for (unsigned i = 0; i < matrix.size(); ++i) {
			for (unsigned j = 0; j < matrix[0].size(); ++j)
				matrixTmp[i][j] = evolve(matrix, i, j);
		}
		copy(matrixTmp, matrix);
	} // nextGeneration()

} // namespace

int main(int argc, char **argv) {
    int i = start_ncurses();
    if(i == 1) return 1;

    string automaton;
    erase();
    do {
		printw("Type which automaton to display ('h' for help) : ");
		automaton = getString();
		if (automaton == "h")
			printw("Automata avalaible : glider, tetris.\n\n");
	} while (automaton != "glider" && automaton != "tetris");

    Matrix matrix = init(10);
    setAutomaton(matrix, automaton);

    /* Alternative instructions :
    keypad(stdscr, TRUE);
    erase();
    show(matrix);
    int ch;
    while((ch = getch()) != KEY_F(1)) {
        erase();
        nextGeneration(matrix);
        show(matrix);
    }
    */

    erase();
    while(true) {
        show(matrix);
        refresh();
	    this_thread::sleep_for(chrono::milliseconds(1000));
        nextGeneration(matrix);
        erase();
    }

    return 0;
}
