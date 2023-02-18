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

// Global & Enums
const float BASE_DAMAGE[5] = {1,1.5,4.5,7.5,8.5};

enum SPECIAL_EVENTS { Shaman = 6, Vajsh = 7, Bowser = 99,
                      MushMario = 11 , MushFibo = 12,
                      RemedyObtained = 15, MaidenKissObtained = 16, PhoenixDownObtained = 17};

// Helper Functions
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

int nextPrime(const int a){
  int i = (a%2==0) ? (a+1) : (a+2);
  while(!isPrime(i)){
    i += 2;
  }
  return i;
}

int prevFibo(const int a){
  int i = 1;
  int j = 1;
  int tempJ;
  while(j < a){
    tempJ = j;
    j = i+j;
    i = tempJ;
  }
  return i;
}

bool isKing(const int MAX_HEALTH){
  return (MAX_HEALTH == 999);
}

bool isLancelot(const int MAX_HEALTH){
  return (isPrime(MAX_HEALTH)) ;
}

int combat(int &level,int &levelO,int &event, const int MAX_HEALTH ,int &HP, int &phoenixdown){

  // WIN: 1
  if ((level > levelO) || isKing(MAX_HEALTH) || isLancelot(MAX_HEALTH)){
    // REGULAR MOBS
    if (event >= 1 && event <=5)
      level = (level +1 ) > 10 ? 10 : level +1;
    
    // BOSSES: Shaman and Vajsh
    else if (event == Shaman || event == Vajsh)
      level = (level +2 ) > 10 ? 10 : level +2;
    return 1;
  }

  // LOSE: 0
  if (level < levelO){
    if(event != Shaman && event != Vajsh){
      int damage;
      damage = BASE_DAMAGE[event-1]*levelO*10;
      cout << " damage: " << damage << "    "; // DEBUG
      HP = HP - damage;
    }
    return 0;
  }
  // DRAW
  return 2;
}

bool isDead(int &HP){
  if(HP <=0 )
    return true;
  return false;
}

bool combatBowser(const int MAX_HEALTH, int &level){
  if (isKing(MAX_HEALTH)) 
    return true;
  
  if (isLancelot(MAX_HEALTH) && level >= 8)
    return true;

  if (level == 10)
    return true;

  return false;
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

// MAIN ADVENTURE FUNCTION
void adventureToKoopa(string file_input, int & HP, int & level, int & remedy, int & maidenkiss, int & phoenixdown, int & rescue) {
  ifstream input_file(file_input);
  string line1, line2;
  // line1 : Is Stat, line2 : is Events
  getline(input_file, line1);
  istringstream stats(line1);
  stats >> HP >> level >> remedy >> maidenkiss >> phoenixdown;
  getline(input_file, line2); 
  istringstream events(line2);

  // TINY STATE
  bool tinyState = false;
  int tinyRemain = 0;

  // FROG STATE 
  bool frogState = false;
  int  frogRemain = 0;
  int levelBeforeFrog;

  // Journey starts
  rescue = -1;
  const int MAX_HEALTH = HP;

  int event;
  int i = 1;
  while(events >> event){
    cout << event << " "; // DEBUG
    switch (event) {
    
    /*------------------------ 
       THIS AREA IS MAINLY
            PRE-COMBAT & MISCS
    --------------------------*/
    // SUCCESSFUL
    case 0:{
      rescue = 1;
      display(HP, level, remedy, maidenkiss, phoenixdown, rescue);
      return;
    }
    
    /*--------------------- 
       THIS AREA IS MAINLY
          MUSHROOMS EVENTS
    ----------------------*/
    case MushMario:{
      int n1 = ((level + phoenixdown)%5 + 1)*3;
      int s1 = n1*(100-n1);

      HP += s1%100; 
      int nextPrimeHP = nextPrime(HP);
      HP = (nextPrimeHP > MAX_HEALTH) ? MAX_HEALTH : nextPrimeHP;
      break;
    }

    case MushFibo:{
      int fiboHP = prevFibo(HP);
      HP = fiboHP > 1 ? fiboHP : 1;
      break;
    }

    /*--------------------- 
       THIS AREA IS MAINLY
            POTIONS EVENTS
    ----------------------*/
    case RemedyObtained:{
      if(tinyState)
        tiny_cleanse(tinyState, tinyRemain, HP, MAX_HEALTH);
      else
        remedy = (remedy + 1 )> 99 ? 99 : remedy + 1 ;
      break;
    } // --> REMEDY

    case MaidenKissObtained:{
      if(frogState)
        frog_cleanse(frogState, frogRemain, level, levelBeforeFrog);
      else
        maidenkiss = (maidenkiss + 1 )> 99 ? 99 : maidenkiss + 1 ;
      break;
    } // --> MAIDENKISS

    case PhoenixDownObtained:{
      phoenixdown = (phoenixdown + 1 )> 99 ? 99 : phoenixdown + 1 ;
      break;
    }

    /*--------------------- 
       THIS AREA IS MAINLY
            COMBAT
    ----------------------*/

    // 1 -- 5 and 6 -- 7 EVENTS 
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case Shaman:
    case Vajsh: {
      // SKIP
      if (event == Shaman || event == Vajsh){
        if(tinyState || frogState) {
          break;
        }
      }
      
      // ENTER COMBAT
      int b = i%10;
      int levelO = (i>6) ? ((b>5)?b:5) : b;
      int victory = combat(level, levelO, event, MAX_HEALTH, HP, phoenixdown);
      cout << "Mob's lv : " << levelO << " "; // DEBUG

      // LOSE TO SHAMAN
      if (event == Shaman && victory == 0){
        tiny_morphed(tinyState, tinyRemain, HP);
        if(remedy){
          tiny_cleanse(tinyState, tinyRemain, HP, MAX_HEALTH);
          remedy--;
        }
      }

      // LOSE TO Vajsh
      else if (event == Vajsh && victory == 0){
        // SAVE LEVEL BEFORE FROG-ed
        levelBeforeFrog = level; 
        frog_morphed(frogState, frogRemain, level);
        if(maidenkiss){
          frog_cleanse(frogState, frogRemain, level, levelBeforeFrog);
          maidenkiss--;
        }
      }
      break;
    }
    
    // BOWSER FIGHT
    case Bowser:{
      bool winBowser = combatBowser(MAX_HEALTH, level);
      if (winBowser)
        level = 10;
      else{
        rescue = 0;
        display(HP, level, remedy, maidenkiss, phoenixdown, rescue);
        return;
      }
      break;
    }

    default:
      goto skip;
    }

    /*--------------------- 
       THIS AREA IS MAINLY
          POST-COMBAT
    ----------------------*/
    // ON DEATH
    if (isDead(HP)){
      if(phoenixdown){
        phoenixdown--;
        HP = MAX_HEALTH;
      }
      else{
        rescue = 0;
        display(HP, level, remedy, maidenkiss, phoenixdown, rescue);
        return;
      }
    }

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

  skip:
    continue;
  }

  // AT THIS STATE, NO MORE EVENTS
  rescue = 1;
  display(HP, level, remedy, maidenkiss, phoenixdown, rescue);

  input_file.close();
}
