#include "knight.h"

int main(int argc, char ** argv) {
    string file_input(argv[1]);
    // Initiate the variables
    ifstream input_file(file_input);
    int HP, level, remedy, maidenkiss, phoenixdown, rescue;
    input_file >> HP >> level >> remedy >> maidenkiss >> phoenixdown;
    rescue = -1;
    input_file.close();
    // The Game Begins
    display(HP, level, remedy, maidenkiss, phoenixdown, rescue);
    adventureToKoopa(file_input, HP, level, remedy, maidenkiss, phoenixdown, rescue);

    return 0;
}
