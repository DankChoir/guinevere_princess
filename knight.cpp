#include "knight.h"

void display(int HP, int level, int remedy, int maidenkiss, int phoenixdown, int rescue) {
  cout << "HP=" << HP
      << ", level=" << level
      << ", remedy=" << remedy
      << ", maidenkiss=" << maidenkiss
      << ", phoenixdown=" << phoenixdown
      << ", rescue=" << rescue << endl;
}

// Global & Enums
const float BASE_DAMAGE[5] = {1,1.5,4.5,7.5,9.5};

enum SPECIAL_EVENTS { Shaman = 6, Vajsh = 7, Bowser = 99,
                      MushMario = 11 , MushFibo = 12,
                      RemedyObtained = 15, MaidenKissObtained = 16, PhoenixDownObtained = 17,
                      Merlin = 18, Asclepius = 19};

// Helper Functions
bool isPrime(const int a){
  if (a<=1) return false;
  if (a==2) return true;

  for(int i=2; i*i <= a; i++){
    if(a%i==0) return false;
  } return true;
}

int nextPrime(const int a){
  int i = (a%2==0) ? (a+1) : (a+2);
  while(!isPrime(i)){i += 2;}
  return i;
}

int prevFibo(const int a){
  int tempJ, i = 1, j=1;
  while(j < a){
    tempJ = j;
    j = i+j;
    i = tempJ;
  } return i;
}

bool isKing(const int MAX_HEALTH){
  return (MAX_HEALTH == 999);
}

bool isLancelot(const int MAX_HEALTH){
  return (isPrime(MAX_HEALTH)) ;
}

bool isMushGhost(const int eventCode){
  string eventCode_str = to_string(eventCode);
  string firstTwo = eventCode_str.substr(0,2);
  return (firstTwo == "13");
}

bool hasAllMerlinLetters(string itemName) {
  const string merlinName = "merlin";
  int foundCount[6] = { 0 };

  for (char c : itemName) {
    for (int i = 0; i < 6; i++) {
      if (tolower(c) == tolower(merlinName[i])) {
        foundCount[i]++;
        break;
      }
    }
  }

  for (int i = 0; i < 6; i++) {
    if (foundCount[i] == 0) {
      return false;
    }
  }

  return true;
}

//COMBAT-WISE
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
      HP = HP - damage;
    }
    return 0;
  }
  // DRAW
  return 2;
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

// STATE CHECK
bool isDead(int &HP){
  if(HP <=0 )
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
  HP *= 5;
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

void mushTypeOne(int &HP, int* arr, int n){
  int maxi = 0, mini = 0;
  for(int i = 0; i <n; i++){
    int value = arr[i];
    // use =, as we need the last pos   
    maxi = (value >= arr[maxi]) ? i : maxi;
    mini = (value <= arr[mini]) ? i : mini;
  }
  HP -= (maxi+mini);
}

void mushTypeTwo(int &HP, int* arr, int n){
  int mtx , mti ;
  bool descended = false;

  for(int i =1; i < n; i++){
    bool ascending = arr[i] > arr[i-1];
    
     // HAVE NOT ON DOWN HILL yet
    if (!descended){
      if (!ascending) {
        mti = i-1;
        mtx = arr[i-1];
        descended = (arr[i] < arr[i-1]) ? true : false;
      }
    }
        
    // EITHER (uphill + not yet descended) or (downhill + descended)    
    if(!(ascending^descended)){ 
      mti = -3;
      mtx = -2;
      HP -= (mtx + mti) ;
      return;
    }
  }
  // STILL, can be a ramp moutain
  if(!descended){
    mti = n-1;
    mtx = arr[n-1];
    HP -= (mtx + mti) ;
    return;
  }
  HP -= (mtx + mti) ;
  return;
}

void mushTypeThree(int &HP, int* arr, int n){
  int maxi = 0, mini = 0;
  int *arr2 = new int[n];
  for(int i = 0; i <n; i++){
    arr2[i] = arr[i];
    arr2[i] = arr2[i] < 0 ? (-arr2[i]) : arr2[i];
    arr2[i] = (17*arr2[i]+9)%257;  
  }

  for(int i = 0; i <n; i++){
    int value = arr2[i];
    maxi = (value > arr2[maxi]) ? i : maxi;
    mini = (value < arr2[mini]) ? i : mini;
  }
  delete [] arr2;
  HP -= (maxi+mini);
}

void mushTypeFour(int &HP, int* arr, int n) {
  int max2_3x = -5;
  int max2_3i = -7;
  
  int *arr2 = new int[n];
  for(int i = 0; i <n; i++){
    arr2[i] = arr[i];
    arr2[i] = arr2[i] < 0 ? (-arr2[i]) : arr2[i];
    arr2[i] = (17*arr2[i]+9)%257;  
    // cout << " |" << arr2[i] << "  ";
  }

  if (n >= 3) {
    int max1 = arr2[0];
    int max2 = -2147483647;
    int max1i = 0;
    int max2i = -1;

    for (int i = 1; i < 3; i++) {
        if (arr2[i] > max1) {
            max2 = max1;
            max2i = max1i;
            max1 = arr2[i];
            max1i = i;
        } else if (arr2[i] > max2) {
            max2 = arr2[i];
            max2i = i;
        }
    }

    if (max2i != -1) {
        max2_3x = arr2[max2i];
        max2_3i = max2i;
    }
  }
  delete [] arr2;
  HP -= (max2_3x+max2_3i);
}

// MAIN ADVENTURE FUNCTION
void adventureToKoopa(string file_input, int & HP, int & level, int & remedy, int & maidenkiss, int & phoenixdown, int & rescue) {
  string mushGhost, asclepiusPack, merlinPack;

  ifstream input_file(file_input);
  string line1, line2, line3;
  
  getline(input_file, line1);
  getline(input_file, line2); 
  getline(input_file, line3); 

  istringstream stats(line1);
  istringstream events(line2);
  istringstream fileNames(line3);
  stats >> HP >> level >> remedy >> maidenkiss >> phoenixdown;

  getline(fileNames, mushGhost, ',');
  getline(fileNames, asclepiusPack, ',');
  fileNames >> merlinPack;
  // TINY STATE
  bool tinyState = false;
  int tinyRemain = 0;

  // FROG STATE 
  bool frogState = false;
  int  frogRemain = 0;
  int levelBeforeFrog;

  // Asclepius and Merlin encountered
  bool metAsclepius = false;
  bool metMerlin = false;

  // Journey starts
  int events_nums = 0;
  int event_index = 0;

  rescue = -1;
  const int MAX_HEALTH = HP;
  int event;
  int i = 1;

  while (events >> event) {
    events_nums++;
  }

  // reset the stream to the beginning
  events.clear();
  events.seekg(0, std::ios::beg);

  while(events >> event){
    event_index++;
    /*--------------------------
       THIS AREA IS PARTICULARLY 
          JUST for MUSH GHOST
    ----------------------------*/
    if(isMushGhost(event)){
      ifstream ghostfFile(mushGhost);
      string dong;

      getline(ghostfFile,dong);
      int amount = stoi(dong);
      int *nums = new int[amount];

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
       * READ NUMBERS FROM mushGhostFile
       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
      getline(ghostfFile,dong);
      istringstream numbers_file(dong);
      
      int num;
      for (int i = 0; i < amount; i++) {
        string num_str;
        getline(numbers_file, num_str, ',');

        num = stoi(num_str);
        nums[i] = num;
      }
      
      /*~~~~~~~~~~~~~~~~~~~~~~
       * MAIN MushGhost EVENTS
       ~~~~~~~~~~~~~~~~~~~~~~~*/
      string mushGhostEvents = to_string(event);
      for(int i = 2; i < mushGhostEvents.length();i++){
        char eventCode = mushGhostEvents[i];

        switch(eventCode){
          case '1':{
            mushTypeOne(HP, nums, amount);
            break;
          }
          case '2':{
            mushTypeTwo(HP, nums, amount);
            break;
          }
          case '3':{
            mushTypeThree(HP, nums, amount);
            break;
          }
          case '4':{
            mushTypeFour(HP, nums, amount);
            break;
          }
          default:
            cout << endl;
        }

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
      }

      display(HP, level, remedy, maidenkiss, phoenixdown, rescue);
      /*~~~~~~~~~~~~
       * POST EVENTS
       ~~~~~~~~~~~~~*/
      delete[] nums;
      ghostfFile.close();
      i++;
      continue;
    }

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
      HP = nextPrimeHP;
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
        remedy++;
      break;
    } // --> REMEDY

    case MaidenKissObtained:{
      if(frogState)
        frog_cleanse(frogState, frogRemain, level, levelBeforeFrog);
      else
        maidenkiss++; 
      break;
    } // --> MAIDENKISS

    case PhoenixDownObtained:{
      phoenixdown++;
      break;
    }

    /*--------------------- 
       THIS AREA IS MAINLY
            COMBAT
    ----------------------*/
    case Merlin:{ 
      if(metMerlin)
        break;

      ifstream enchanted_file(merlinPack);
      string itemName;
      getline(enchanted_file,itemName);
      int n9 = stoi(itemName);

      for(int i =0; i < n9;i++){
        getline(enchanted_file,itemName);
        if (hasAllMerlinLetters(itemName)){
          if (itemName.find("Merlin") != string::npos || itemName.find("merlin") != string::npos) {
            HP += 3;
          }
          else{
            HP += 2;
          }
        }
      }

      metMerlin = true;
      enchanted_file.close();
      break;
    }
    case Asclepius:{
      if(metAsclepius)
        break; // display() yes

      ifstream potion_file(asclepiusPack);
      string poitionLine;
      int r1, c1;

      // GET ROW AND COLUMN
      getline(potion_file,poitionLine);
      r1 = stoi(poitionLine);
      getline(potion_file,poitionLine);
      c1 = stoi(poitionLine);

      for(int r=0; r <r1; r++){
        int limit = 0;
        getline(potion_file,poitionLine);
        istringstream potions(poitionLine);
        int potion;
        for(int c=0; c <c1; c++){
          potions >> potion;
          if (limit == 3)
            break;
          if (potion == 16){
            remedy++;
            limit++;
          }
          else if (potion == 17){
            maidenkiss++;
            limit++;
          }
          else if (potion == 18){
            phoenixdown++;
            limit++;
          }
        }
      }
      metAsclepius = true;
      potion_file.close();
      break;
    }
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

      // LOSE TO SHAMAN
      if (event == Shaman && victory == 0){
        tiny_morphed(tinyState, tinyRemain, HP);
      }

      // LOSE TO Vajsh
      else if (event == Vajsh && victory == 0){
        // SAVE LEVEL BEFORE FROG-ed
        levelBeforeFrog = level; 
        frog_morphed(frogState, frogRemain, level);
      }
      break;
    }
    
    // BOWSER FIGHT
    case Bowser:{
      bool winBowser = combatBowser(MAX_HEALTH, level);
      if (!winBowser){
        rescue = 0;
        display(HP, level, remedy, maidenkiss, phoenixdown, rescue);
        return;
      }
      level = 10;
      break;
    }

    default:
      break;
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

    // Checking DE-BUFFs : Tiny & Frog State
    if(tinyRemain){
      if(remedy){
        tiny_cleanse(tinyState, tinyRemain, HP, MAX_HEALTH);
        remedy--;
      }
      else
        tinyRemain--;
    }
    else if (tinyState && tinyRemain == 0){
      tiny_cleanse(tinyState, tinyRemain, HP, MAX_HEALTH);
    }

    if(frogRemain){
      if(maidenkiss){
        frog_cleanse(frogState, frogRemain, level, levelBeforeFrog);
        maidenkiss--;
      }
      else
        frogRemain--;
    }
    else if (frogState && frogRemain == 0){
      frog_cleanse(tinyState, tinyRemain, level, levelBeforeFrog);
    }

    // Checking POTIONS
    remedy = remedy > 99 ? 99 : remedy;
    maidenkiss = maidenkiss > 99 ? 99 : maidenkiss;
    phoenixdown = phoenixdown > 99 ? 99 : phoenixdown;

    // Checking HEALTH
    HP = HP > MAX_HEALTH ? MAX_HEALTH : HP;

    if (event_index == events_nums){
      rescue = 1;
      display(HP, level, remedy, maidenkiss, phoenixdown, rescue);
      return;
    }
    
    // CLI 
    i++;
    display(HP, level, remedy, maidenkiss, phoenixdown, rescue);
    
  }
  input_file.close();
}
