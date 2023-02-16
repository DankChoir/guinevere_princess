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

bool isPrime(const int a){
  if (a<=1)
    return false;
  if (a==2)
    return true;

  for(int i=2; i*i <= a; i++){
    if(a%i==0)
      return false;
  }
  return true;
}

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
      cout << " damage: " << damage << "    "; // DEBUG
      HP = HP - damage;

      if (HP <= 0 && phoenixdown > 0){
        HP = MAX_HEALTH;
        phoenixdown--;
      }
      else if (HP <= 0 && phoenixdown == 0){
        // DEAD: -1
        return -1;
      }
    }
    return 0;
  }
  // DRAW
  return 2;
}

void tiny_morphed(bool &state,int &remaining, int &HP){
  state = true;
  remaining = 3;
  HP = (HP/5 )< 5 ? 1 : HP/5;
}

void tiny_cleanse(bool &state,int &remaining, int &HP, const int MAX_HEALTH){
  state = false;
  remaining = 0;
  HP = HP*5 > MAX_HEALTH ? MAX_HEALTH : HP*5;
}

void frog_morphed(bool &state,int &remaining, int &level){
  state = true;
  remaining = 3;
  level = 1;
}

void frog_cleanse(bool &state, int &remaining, int&level, const int levelBeforeFrog){
  state = false;
  remaining = 0;
  level = levelBeforeFrog;
}

void adventureToKoopa(string file_input, int & HP, int & level, int & remedy, int & maidenkiss, int & phoenixdown, int & rescue) {
  ifstream input_file(file_input);
  string line1, line2;
  const int MAX_HEALTH = HP;

  // TINY STATE
  bool tinyState = false;
  int tinyRemain = 0;

  // FROG STATE 
  bool frogState = false;
  int  frogRemain = 0;
  int levelBeforeFrog;

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
    /*--------------------- 
       THIS AREA IS MAINLY
          MUSHROOMS EVENTS
    ----------------------*/
    else if (event == 11){
      display(HP, level, remedy, maidenkiss, phoenixdown, rescue); // DEBUG DEBUG DEBUG
      int n1 = ((level + phoenixdown)%5 + 1)*3;
      int s1 = n1*(100-n1);
      HP += s1%100; // Then HP is incremented to the nearest prime !!
    } 

    /*--------------------- 
       THIS AREA IS MAINLY
            POTIONS EVENTS
    ----------------------*/
    else if(event == 15){
      if(tinyState)
        tiny_cleanse(tinyState, tinyRemain, HP, MAX_HEALTH);
      else
        remedy = (remedy + 1 )> 99 ? 99 : remedy + 1 ;
    } // --> REMEDY

    else if(event == 16){
      if(frogState)
        frog_cleanse(frogState, frogRemain, level, levelBeforeFrog);
      else
        maidenkiss = (maidenkiss + 1 )> 99 ? 99 : maidenkiss + 1 ;
    } // --> MAIDENKISS

    else if(event == 17)
      phoenixdown = (phoenixdown + 1 )> 99 ? 99 : phoenixdown + 1 ;

    /*--------------------- 
       THIS AREA IS MAINLY
            COMBAT
    ----------------------*/

    // 1 -- 5 EVENTS !! maybe separating 6 7
    else if (event >= 1 && event <=7){
      // SKIP
      if (event == 6 || event ==7){
        if(tinyState || frogState) {
          // display(HP, level, remedy, maidenkiss, phoenixdown, rescue);
          // i++;
          // continue; // ko co frogState tinyState m,ai sua
          goto postfight;
        }
      }
      
      // ENTER COMBAT
      int b = i%10;
      int levelO = (i>6) ? ((b>5)?b:5) : b;
      int victory = combat(level, levelO, event, MAX_HEALTH, HP, phoenixdown);
      cout << "Mob's level : " << levelO << " "; // DEBUG
      // IN CASE DEAD
      if (victory == -1){
        rescue = 0;
        display(HP, level, remedy, maidenkiss, phoenixdown, rescue);
        return;
      }

      // LOSE TO SHAMAN
      else if (event == 6 && victory == 0){
        tiny_morphed(tinyState, tinyRemain, HP);
        if(remedy){
          tiny_cleanse(tinyState, tinyRemain, HP, MAX_HEALTH);
          remedy--;
        }
      }

      // LOSE TO Vajsh
      else if (event == 7 && victory == 0){
        // SAVE LEVEL BEFORE FROG-ed
        levelBeforeFrog = level; 
        frog_morphed(frogState, frogRemain, level);
        if(maidenkiss){
          frog_cleanse(frogState, frogRemain, level, levelBeforeFrog);
          maidenkiss--;
        }
      }
    }

    /*--------------------- 
       THIS AREA IS MAINLY
          POST-COMBAT
    ----------------------*/
    postfight:

      display(HP, level, remedy, maidenkiss, phoenixdown, rescue);
      i++;

      // Checking DE-BUFFs : Tiny & Frog State
      if(tinyRemain)
        tinyRemain--;
      else if (tinyState && tinyRemain == 0){
        tiny_cleanse(tinyState, tinyRemain, HP, MAX_HEALTH);
      }

      if(frogRemain)
        frogRemain--;
      else if (frogState && frogRemain == 0){
        frog_cleanse(tinyState, tinyRemain, level, levelBeforeFrog);
      }

  }

  // AT THIS STATE, NO MORE EVENTS
  rescue = 1;
  display(HP, level, remedy, maidenkiss, phoenixdown, rescue);

  input_file.close();
}
