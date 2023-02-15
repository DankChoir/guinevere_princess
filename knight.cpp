#include "knight.h"
#include <fstream>
#include <sstream>


void display(int HP, int level, int remedy, int maidenkiss, int phoenixdown, int rescue) {
  cout << "HP=" << HP
      << ", level=" << level
      << ", remedy=" << remedy
      << ", maidenkiss=" << maidenkiss
      << ", phoenixdown=" << phoenixdown
      << ", rescue=" << rescue << endl;
}

// Global 
const float BASE_DAMAGE[5] = {1,1.5,4.5,7.5,9.5};

int combat(int &level,int &levelO,int &event, const int MAX_HEALTH ,int &HP, int &phoenixdown){
  // WIN: 1
  if (level > levelO){
    // REGULAR MOBS
    if (event >= 1 && event <=5)
      level = (level +1 ) > 10 ? 10 : level +1;
    
    // BOSSES: Shaman and Vajsh
    else if (event == 6 || event == 7)
      level = (level +2 ) > 10 ? 10 : level +2;
    return 1;
  }

  // LOSE: 0
  if (level < levelO){
    if(event != 6 && event != 7){
      int damage;
      damage = BASE_DAMAGE[event-1]*levelO*10;
      HP = HP - damage;

      if (HP <= 0 && phoenixdown > 0){
        HP = MAX_HEALTH;
        phoenixdown--;
      }
      if (HP <= 0 && phoenixdown == 0){
        // DEAD: -1
        return -1;
      }
    }
    return 0;
  }
  // DRAW
  return 2;
}

void frog_morphed(bool &state,int &remaining, int &HP){
  state = true;
  remaining = 3;
  HP = (HP/5 )< 5 ? 1 : HP/5;
}

void frog_cleanse(bool &state,int &remaining, int &HP, const int MAX_HEALTH){
  state = false;
  remaining = 0;
  HP = HP*5 > MAX_HEALTH ? MAX_HEALTH : HP*5;
}

void adventureToKoopa(string file_input, int & HP, int & level, int & remedy, int & maidenkiss, int & phoenixdown, int & rescue) {
  ifstream input_file(file_input);
  string line1, line2;
  const int MAX_HEALTH = HP;
  bool frogState = false;
  int frogRemain = 0;

  // line1 : Is Stat, line2 : is Events
  getline(input_file, line1); // skip line1
  getline(input_file, line2); 
  istringstream events(line2);

  // Journey starts
  int event;
  int i = 1;
  while(events >> event){
    cout << event << " ";
    
    /*------------------------ 
       THIS AREA IS MAINLY
            PRE-COMBAT & MISCS
    --------------------------*/
    // SUCCESSFUL
    if (event == 0){
      rescue = 1;
      display(HP, level, remedy, maidenkiss, phoenixdown, rescue);
      return;
    }

    // POTIONS AREA, !! implement cleanse
    else if(event == 15)
      remedy = (remedy + 1 )> 99 ? 99 : remedy + 1 ;

    else if(event == 16)
      maidenkiss = (maidenkiss + 1 )> 99 ? 99 : maidenkiss + 1 ;

    else if(event == 17)
      phoenixdown = (phoenixdown + 1 )> 99 ? 99 : phoenixdown + 1 ;

    /*--------------------- 
       THIS AREA IS MAINLY
            COMBAT
    ----------------------*/

    // 1 -- 5 EVENTS, !! minion missing, not yet fam
    else if (event >= 1 && event <=7){
      // SKIP
      if (event == 6 || event ==7){
        if(frogState) // will add minion later
          continue;
      }
      
      // ENTER COMBAT
      int b = i%10;
      int levelO = i>6 ? (b>5?b:5) : b;
      int result = combat(level, levelO, event, MAX_HEALTH, HP, phoenixdown);

      // IN CASE DEAD
      if (result == -1){
        rescue = 0;
        display(HP, level, remedy, maidenkiss, phoenixdown, rescue);
        return;
      }

      // LOSE TO SHAMAN
      if (event == 6 && result == 0){
        if(remedy)
          remedy--;
        else
          frog_morphed(frogState, frogRemain, HP);
      }
    }

    /*--------------------- 
       THIS AREA IS MAINLY
          POST-COMBAT
    ----------------------*/

    display(HP, level, remedy, maidenkiss, phoenixdown, rescue);
    i++;

    // UNDER DE-BUFF
    if(frogRemain)
      frogRemain--;
    else if (frogState && frogRemain == 0){
      frog_cleanse(frogState, frogRemain, HP, MAX_HEALTH);
    }

  }

  // AT THIS STATE, NO MORE EVENTS
  rescue = 1;
  display(HP, level, remedy, maidenkiss, phoenixdown, rescue);

  input_file.close();
}
