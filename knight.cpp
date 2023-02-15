#include "knight.h"
#include <fstream>
#include <sstream>

// Global 
const float BASE_DAMAGE[5] = {1,1.5,4.5,7.5,9.5};


void display(int HP, int level, int remedy, int maidenkiss, int phoenixdown, int rescue) {
    cout << "HP=" << HP
        << ", level=" << level
        << ", remedy=" << remedy
        << ", maidenkiss=" << maidenkiss
        << ", phoenixdown=" << phoenixdown
        << ", rescue=" << rescue << endl;
}

int oneToFive(int &i,int event, const int MAX_HEALTH ,int &HP, int &level, int &phoenixdown){
  // i is the event number
  int b = i%10;
  int levelO = b; 

  if (level > levelO){
    level = (level +1 ) > 10 ? 10 : level +1;
  }

  if (level < levelO){
    int damage;
    damage = BASE_DAMAGE[event-1]*levelO*10;
    HP = HP - damage;

    if (HP <= 0 && phoenixdown > 0){
      HP = MAX_HEALTH;
      phoenixdown--;
    }
    if (HP <= 0 && phoenixdown == 0){
      return -1;
    }
  }
  return 0;
}

void adventureToKoopa(string file_input, int & HP, int & level, int & remedy, int & maidenkiss, int & phoenixdown, int & rescue) {
  ifstream input_file(file_input);
  string line1, line2;
  const int MAX_HEALTH = HP;

  // line1 : Is Stat, line2 : is Events
  getline(input_file, line1); // skip line1
  getline(input_file, line2); 
  istringstream events(line2);

  // Journey start
  int event;
  int i = 1;
  while(events >> event){
    cout << event << " ";

    // SUCCESSFUL
    if (event == 0){
      rescue = 1;
      display(HP, level, remedy, maidenkiss, phoenixdown, rescue);
      return;
    }

    // POTIONS AREA
    else if(event == 15)
      remedy = (remedy + 1 )> 99 ? 99 : remedy + 1 ;

    else if(event == 16)
      maidenkiss = (maidenkiss + 1 )> 99 ? 99 : maidenkiss + 1 ;

    else if(event == 17)
      phoenixdown = (phoenixdown + 1 )> 99 ? 99 : phoenixdown + 1 ;

    // 1 -- 5 EVENTS
    else if (event >= 1 && event <=5){
      int result = oneToFive(i, event, MAX_HEALTH, HP, level, phoenixdown);
      if (result == -1){
        rescue = 0;
        display(HP, level, remedy, maidenkiss, phoenixdown, rescue);
        return;
      }
    }

    display(HP, level, remedy, maidenkiss, phoenixdown, rescue);
    i++;
  }

  // AT THIS STATE, NO MORE EVENTS
  rescue = 1;
  display(HP, level, remedy, maidenkiss, phoenixdown, rescue);

  input_file.close();
}
