// Author: Paul Kassianik
// File: tictactoe.cpp
// Purpose: Create a computer algorithm for playing TicTacToe.
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <cstring>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <locale>

using namespace std;

// Data type declarations

enum PosValue { COMPUTER, DRAW, USER, NA};
enum WhoseMove { CO, US};

struct Position {
  string space_arr[3][3]; // array to hold the value of each space
  Position* parent; // reference to the parent
  Position* children; // array of children. Dynamically allocated in assingArrays function
  PosValue value;  // value of position.
  double totalValue; // sum of losses(5 points) and draws(1 point) divided by the number of availabe spaces in the position.
                     // the smaller the value, the more likely the computer will not lose if it chooses the position.
};

// Function prototypes
void getUserInput(Position* , int);
void computerMove(Position* );
void getValue(Position* );
void assignArrays(Position* , char);
void changeSpace(Position* ,int, int, string);
int getEmptySpaces(Position* );
void displayPosition(Position* );
void fillSpace(Position* , int*, int*, int, char);
void fillWithWhitespace(Position*);
void isDanger(int*, Position*, int*, int*);

int main() {
  Position currentPosition;
  WhoseMove move = US; // keep track of whose move is it
  int moveNum = 1;

  currentPosition.value = NA;
  fillWithWhitespace(&currentPosition);

  // while nobody wins get input from user then computer
  while (currentPosition.value==NA) {
    currentPosition.parent = 0;
    getValue(&currentPosition);

    if (move==US) {

      getUserInput(&currentPosition, moveNum);
      move = CO;
      getValue(&currentPosition);
    }
    else {
      computerMove(&currentPosition);
      move = US;
      getValue(&currentPosition);
    }
    moveNum++;
  }

  // when the game is over, display the final position
  displayPosition(&currentPosition);

  if (currentPosition.value==USER)
    cout << endl << "You Win!"<<endl;
  else if (currentPosition.value==COMPUTER)
    cout << endl << "You Lose."<<endl;
  else
    cout << endl << "Draw."<<endl;

  cout << endl << "Thanks for playing!"<<endl;

  return 0;
}

/*************************************************************
The getUserInput function gets user input, validates it, and
changes the current position.
*************************************************************/

void getUserInput(Position* cur_pos, int move) {
  char userInput[2];
  int row, col;
  bool goodInput = false;

  displayPosition(cur_pos);
  cout << "Enter the field you would want to play in the following format: \n"
       << "(row,column). For example, b1 would mean row b, column 1.\n";
  if (move==1)
    cout << "Remeber, no corners on the first move.\n";
  cout << "Enter your choice: ";
  while (!goodInput) {
    cin >> userInput;
    row = (userInput[0]-'0')-48;
    col = userInput[1]-'0';
    if (move==1) {
      if (row<4 && row>0 && col<4 && row>0 && (cur_pos->space_arr[row-1][col-1])[0]==' '){ // test for valid input
        // on first move, make sure the user doesn't choose a corner
        if (!(row==1 && col==1) && !(row==1 && col==3) && !(row==3 && col==1) && !(row==3 && col==3)) {
          changeSpace(cur_pos, row-1, col-1, "X");
          goodInput = true;
        }
        else {
          cout << "Error: Please enter valid field coordinates."<<endl;
          cout << "Enter your choice: ";
        }
      }
    }
    else {
      if (row<4 && row>0 && col<4 && row>0 && (cur_pos->space_arr[row-1][col-1])[0]==' '){
        changeSpace(cur_pos, row-1, col-1, "X");
        goodInput = true;
      }
      else {
        cout << "Error: Please enter valid field coordinates."<<endl;
        cout << "Enter your choice: ";
      }
    }
  }
}

/***********************************************************************************
The computerMove function selects the best move out of the tree built by
the assignArrays function
***********************************************************************************/

void computerMove(Position* cur_pos) {
  assignArrays(cur_pos, 'c');
  double smallest_value = 10000;
  int isDangerous = 2;
  int index = -1;
  for (int i=0; i<getEmptySpaces(cur_pos); i++){
    if (cur_pos->children[i].totalValue<largest_value) {
      smallest_value = cur_pos->children[i].totalValue;
      index = i;
    }
  }

  int toChangeRow = -1, toChangeCol = -1;
  isDanger(&isDangerous, cur_pos, &toChangeRow, &toChangeCol );

  if (isDangerous == 1){
    for (int i=0; i<getEmptySpaces(cur_pos); i++){
      if ((cur_pos->children[i].space_arr[toChangeRow][toChangeCol])[0] != ' '){
        index = i;
        break;
      }
    }
  }
  else if (isDangerous==0) {
    for (int i=0; i<getEmptySpaces(cur_pos); i++){
      if ((cur_pos->children[i].space_arr[toChangeRow][toChangeCol])[0] != ' '){
        index = i;
        break;
      }
    }
  }

  for (int i=0; i<3; i++){
    for (int j=0; j<3; j++){
      cur_pos->space_arr[i][j] = cur_pos->children[index].space_arr[i][j];
    }
  }
}

/****************************************************************
the assignArrays function builds the tree that's to be used
by the computerMove function
****************************************************************/

void assignArrays(Position* pos, char userOrComp) {
  int last_row = -1, last_col = -1;
  int whiteSpaces = getEmptySpaces(pos); // get the number of spaces
  pos->children = new Position[whiteSpaces]; // dynamically instantiate an array of children
  for (int i=0; i < whiteSpaces; i++){
    fillSpace(pos,&last_row,&last_col, i, userOrComp); // fill next space with character (u or c)
    pos->children[i].parent = pos;

    getValue(&(pos->children[i])); // assign a value to the child
    if (userOrComp=='c'){
      if (pos->children[i].value == NA) { // if the value is still NA, repeat the function for the child
        assignArrays(&(pos->children[i]), 'u');
      }
    }
    else {
      if (pos->children[i].value == NA) {
        assignArrays(&(pos->children[i]), 'c');
      }
    }
  }

  for (int i=0; i<whiteSpaces; i++) {
    if (pos->children[i].value > pos->value || pos->value == NA){
      pos->value = pos->children[i].value; // assign the pos->value to the largest value of children (worst case scenario)
    }
    pos->totalValue += pos->children[i].totalValue;
  }
  pos->totalValue = pos->totalValue / whiteSpaces;
}

/******************************************************************
The fillSpace function finds the next space in a position and
adds a positin to pos->children
*******************************************************************/
void fillSpace(Position* pos, int* lrow, int* lcol, int childNum, char userOrComp) {
  bool getOut = false;
  bool isSet = false;

  if (*lrow == -1 && *lcol == -1){
    for (int i=0; i<3; i++){
      for (int j=0; j<3; j++){
        string str = pos->space_arr[i][j];
        if (str[0]==' ') {
          *lrow = i;
          *lcol = j;
          getOut = true;
        }
        if (getOut)
          break;
      }
      if (getOut)
          break;
    }
  }
  else {
    for (int i=*lrow; i<3; i++){
      for (int j=0; j<3; j++) {
        if (i==*lrow && !isSet){
          j = *lcol;
          isSet = true;
        }
        string str = pos->space_arr[i][j];
        if (str[0]==' ' && (i!=*lrow || j!=*lcol)) {
          *lrow = i;
          *lcol = j;
          getOut = true;
        }
        if (getOut)
          break;
      }
      if (getOut)
          break;
    }
  }

  for (int i=0; i<=2; i++){
    for (int j=0; j<=2; j++) {
      if (i==*lrow && j==*lcol) {

        if (userOrComp=='c')
          changeSpace(&(pos->children[childNum]),i,j,"O");
        else
          changeSpace(&(pos->children[childNum]),i,j,"X");
      }
      else {
        changeSpace(&(pos->children[childNum]), i, j, pos->space_arr[i][j]);
      }
    }
  }
}

/***************************************************************
The displayPosition function displays the position passed in.
***************************************************************/
void displayPosition(Position* cur_pos) {
  for (int i=0; i<3; i++){
    if (i!=0 )
      cout << "  _____ _____ _____"<<endl<<endl;
    else
      cout << "   1     2     3"<<endl;
    cout <<endl << (char)(97+(i))<<"  "<<cur_pos->space_arr[i][0]<<"  |  "<<cur_pos->space_arr[i][1]
         << "  |  "<<cur_pos->space_arr[i][2]<<endl;
    if (i==2)
      cout << endl;
  }
}

/*************************************************************
The fillWithWhiteSpace function fills a position with whitespace.
This function is called only once in the beginning of execution.
*************************************************************/
void fillWithWhitespace(Position* cur_pos) {
  for (int i=0; i<3; i++){
    for (int j=0; j<3; j++) {
      cur_pos->space_arr[i][j] = " ";
    }
  }
}

/**************************************************************
The getValue function assigns value of a position (cur_pos).
**************************************************************/
void getValue(Position* cur_pos){
  char checkVal = 'X';
  PosValue val = NA;
  string arr[3][3];
  for (int i=0; i<3; i++){
    for (int j=0; j<3; j++){
      arr[i][j].assign(cur_pos->space_arr[i][j]);
    }
  }

  for (int i=0; i<3; i++) {
    if ((arr[i][0])[0]==checkVal && (arr[i][1])[0]==checkVal && (arr[i][2])[0]==checkVal)
      val = USER;
    if ((arr[0][i])[0]==checkVal && (arr[1][i])[0]==checkVal && (arr[2][i])[0]==checkVal)
      val = USER;
  }

  if ((arr[0][0])[0]==checkVal && (arr[1][1])[0]==checkVal && (arr[2][2])[0]==checkVal)
    val=USER;
  if ((arr[0][2])[0]==checkVal && (arr[1][1])[0]==checkVal && (arr[2][0])[0]==checkVal)
    val=USER;

  checkVal ='O';

  for (int i=0; i<3; i++) {
    if ((arr[i][0])[0]==checkVal && (arr[i][1])[0]==checkVal && (arr[i][2])[0]==checkVal)
      val = COMPUTER;
    if ((arr[0][i])[0]==checkVal && (arr[1][i])[0]==checkVal && (arr[2][i])[0]==checkVal)
      val = COMPUTER;
  }

  if ((arr[0][0])[0]==checkVal && (arr[1][1])[0]==checkVal && (arr[2][2])[0]==checkVal)
    val=COMPUTER;
  if ((arr[0][2])[0]==checkVal && (arr[1][1])[0]==checkVal && (arr[2][0])[0]==checkVal)
    val=COMPUTER;

  if (val==NA && getEmptySpaces(cur_pos)==0)
    val=DRAW;

  cur_pos->value = val;


  if (cur_pos->value == USER) {
    cur_pos->totalValue = cur_pos->value+5;
  }
  else if (cur_pos->value != NA){
    cur_pos->totalValue = cur_pos->value;
  }
  else {
    cur_pos->totalValue = 0;
  }
}


/**********************************************************
The getEmptySpaces function returns the amount of spaces
in a position (pos).
**********************************************************/
int getEmptySpaces(Position* pos) {
  int return_count = 0;
  for (int i=0; i<3; i++){
    for (int j=0; j<3; j++){
       if (pos->space_arr[i][j]==" ") {
         return_count++;
       }
    }
  }

  return return_count;
}

/************************************************************
The changeSpace function changes the space at row r and column
c in pos->space_arr.
************************************************************/
void changeSpace(Position* pos, int r, int c, string val) {
  pos->space_arr[r][c] = val;
}

/***********************************************************
The isDanger function checks if the user has a chance to win
or if the computer has a chance to win.
***********************************************************/
void isDanger(int* isDangerous, Position* cur_pos, int* changeRow, int* changeCol){

  // Check if the user has a chance to win

  for (int r=0; r<3; r++){
    if ((cur_pos->space_arr[r][0])[0]=='X' && (cur_pos->space_arr[r][1])[0]=='X' && (cur_pos->space_arr[r][2])[0]==' ') {
      *changeRow = r;
      *changeCol = 2;
      *isDangerous = 1;
    }
    if ((cur_pos->space_arr[r][1])[0]=='X' && (cur_pos->space_arr[r][2])[0]=='X' && (cur_pos->space_arr[r][0])[0]==' ') {
      *changeRow = r;
      *changeCol = 0;
      *isDangerous = 1;
    }
    if ((cur_pos->space_arr[r][0])[0]=='X' && (cur_pos->space_arr[r][2])[0]=='X' && (cur_pos->space_arr[r][1])[0]==' ') {
      *changeRow = r;
      *changeCol = 1;
      *isDangerous = 1;
    }
  }

  for (int c=0; c<3; c++) {
    if ((cur_pos->space_arr[0][c])[0]=='X' && (cur_pos->space_arr[1][c])[0]=='X' && (cur_pos->space_arr[2][c])[0]==' ') {
      *changeRow = 2;
      *changeCol = c;
      *isDangerous = 1;
    }
    if ((cur_pos->space_arr[1][c])[0]=='X' && (cur_pos->space_arr[2][c])[0]=='X' && (cur_pos->space_arr[0][c])[0]==' ') {
      *changeRow = 0;
      *changeCol = c;
      *isDangerous = 1;
    }
    if ((cur_pos->space_arr[0][c])[0]=='X' && (cur_pos->space_arr[2][c])[0]=='X' && (cur_pos->space_arr[1][c])[0]==' ') {
      *changeRow = 1;
      *changeCol = c;
      *isDangerous = 1;
    }
  }

  if ((cur_pos->space_arr[0][0])[0]=='X' && (cur_pos->space_arr[1][1])[0]=='X' && (cur_pos->space_arr[2][2])[0]==' ') {
      *changeRow = 2;
      *changeCol = 2;
      *isDangerous = 1;
  }
  if ((cur_pos->space_arr[1][1])[0]=='X' && (cur_pos->space_arr[2][2])[0]=='X' && (cur_pos->space_arr[0][0])[0]==' ') {
      *changeRow = 0;
      *changeCol = 0;
      *isDangerous = 1;
  }
  if ((cur_pos->space_arr[0][0])[0]=='X' && (cur_pos->space_arr[2][2])[0]=='X' && (cur_pos->space_arr[1][1])[0]==' ') {
      *changeRow = 1;
      *changeCol = 1;
      *isDangerous = 1;
  }


  if ((cur_pos->space_arr[0][2])[0]=='X' && (cur_pos->space_arr[1][1])[0]=='X' && (cur_pos->space_arr[2][0])[0]==' ') {
      *changeRow = 2;
      *changeCol = 0;
      *isDangerous = 1;
  }
  if ((cur_pos->space_arr[1][1])[0]=='X' && (cur_pos->space_arr[2][0])[0]=='X' && (cur_pos->space_arr[0][2])[0]==' ') {
      *changeRow = 0;
      *changeCol = 2;
      *isDangerous = 1;
  }
  if ((cur_pos->space_arr[2][0])[0]=='X' && (cur_pos->space_arr[0][2])[0]=='X' && (cur_pos->space_arr[1][1])[0]==' ') {
      *changeRow = 1;
      *changeCol = 1;
      *isDangerous = 1;
  }

  // Check if computer has a chance to win

  for (int r=0; r<3; r++){
    if ((cur_pos->space_arr[r][0])[0]=='O' && (cur_pos->space_arr[r][1])[0]=='O' && (cur_pos->space_arr[r][2])[0]==' ') {
      *changeRow = r;
      *changeCol = 2;
      *isDangerous = 0;
    }
    if ((cur_pos->space_arr[r][1])[0]=='O' && (cur_pos->space_arr[r][2])[0]=='O' && (cur_pos->space_arr[r][0])[0]==' ') {
      *changeRow = r;
      *changeCol = 0;
      *isDangerous = 0;
    }
    if ((cur_pos->space_arr[r][0])[0]=='O' && (cur_pos->space_arr[r][2])[0]=='O' && (cur_pos->space_arr[r][1])[0]==' ') {
      *changeRow = r;
      *changeCol = 1;
      *isDangerous = 0;
    }
  }

  for (int c=0; c<3; c++) {
    if ((cur_pos->space_arr[0][c])[0]=='O' && (cur_pos->space_arr[1][c])[0]=='O' && (cur_pos->space_arr[2][c])[0]==' ') {
      *changeRow = 2;
      *changeCol = c;
      *isDangerous = 0;
    }
    if ((cur_pos->space_arr[1][c])[0]=='O' && (cur_pos->space_arr[2][c])[0]=='O' && (cur_pos->space_arr[0][c])[0]==' ') {
      *changeRow = 0;
      *changeCol = c;
      *isDangerous = 0;
    }
    if ((cur_pos->space_arr[0][c])[0]=='O' && (cur_pos->space_arr[2][c])[0]=='O' && (cur_pos->space_arr[1][c])[0]==' ') {
      *changeRow = 1;
      *changeCol = c;
      *isDangerous = 0;
    }
  }

  if ((cur_pos->space_arr[0][0])[0]=='O' && (cur_pos->space_arr[1][1])[0]=='O' && (cur_pos->space_arr[2][2])[0]==' ') {
      *changeRow = 2;
      *changeCol = 2;
      *isDangerous = 0;
  }
  if ((cur_pos->space_arr[1][1])[0]=='O' && (cur_pos->space_arr[2][2])[0]=='O' && (cur_pos->space_arr[0][0])[0]==' ') {
      *changeRow = 0;
      *changeCol = 0;
      *isDangerous = 0;
  }
  if ((cur_pos->space_arr[0][0])[0]=='O' && (cur_pos->space_arr[2][2])[0]=='O' && (cur_pos->space_arr[1][1])[0]==' ') {
      *changeRow = 1;
      *changeCol = 1;
      *isDangerous = 0;
  }


  if ((cur_pos->space_arr[0][2])[0]=='O' && (cur_pos->space_arr[1][1])[0]=='O' && (cur_pos->space_arr[2][0])[0]==' ') {
      *changeRow = 2;
      *changeCol = 0;
      *isDangerous = 0;
  }
  if ((cur_pos->space_arr[1][1])[0]=='O' && (cur_pos->space_arr[2][0])[0]=='O' && (cur_pos->space_arr[0][2])[0]==' ') {
      *changeRow = 0;
      *changeCol = 2;
      *isDangerous = 0;
  }
  if ((cur_pos->space_arr[2][0])[0]=='O' && (cur_pos->space_arr[0][2])[0]=='O' && (cur_pos->space_arr[1][1])[0]==' ') {
      *changeRow = 1;
      *changeCol = 1;
      *isDangerous = 0;
  }


}
