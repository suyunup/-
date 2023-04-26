// #include <ncurses.h>
// #include <unistd.h>  /* only for sleep() */
#include <string>
#include <iostream>
using namespace std;

int main(void){
  int number = 48;
  cout << (char)number;
  // string sequence[4] = {"1342", "2431", "3214", "4123"};
  // for(int i = 0; i < 4; i++){
  //   if(sequence[i].at(0) == '3'){
  //     number = i;
  //     break;
  //   }
  // }
  // for(int i = 0; i < 4; i++){
  //   switch(sequence[number].at(i)){
  //     case '1':
  //       cout << "1";
  //       break;
  //     case '2':
  //       cout << "2";
  //       break;
  //     case '3':
  //       cout << "3";
  //       break;
  //     case '4':
  //       cout << "4";
  //       break;
  //   }
  // }
  return 0;
}
// #include <ncurses.h>
// #include <unistd.h>
// static inline bool
// kbhit(void){
//     int ch;
//     bool ret;
//
//     nodelay(stdscr, TRUE);
//
//     ch = getch();
//     if ( ch == ERR ) {
//         ret = false;
//     } else {
//         ret = true;
//         ungetch(ch); // 마지막에 받은 문자를 버퍼에 다시 넣어서 다음 getch()가 받을 수 있도록 합니다.
//     }
//
//     nodelay(stdscr, FALSE);
//     return ret;
// }
// int timer(){
//     int row = 10, col = 10;
//     initscr();
//     noecho(); // 입력을 자동으로 화면에 출력하지 않도록 합니다.
//     curs_set(FALSE); // cursor를 보이지 않게 합니다.
//
//     keypad(stdscr, TRUE);
//     while(1){
//         int input = getch();
//         clear();
//         switch(input){
//             case KEY_UP:
//             mvprintw(--row, col, "A"); // real moving in your screen
//             continue;
//             case KEY_DOWN:
//             mvprintw(++row, col, "A");
//             continue;
//             case KEY_LEFT:
//             mvprintw(row, --col, "A");
//             continue;
//             case KEY_RIGHT:
//             mvprintw(row, ++col, "A");
//             continue;
//
//         }
//         if(input == 'q') break;
//     }
//
//     endwin();
//     return 0;
// }
//
// int main(){
//     return timer();
// }

















void meet_gate(int x, int y, int dir){
    point input_gate, output_gate;
    if(first_gate.x == x && first_gate.y == y){
      input_gate = point(x, y);
      output_gate = point(second_gate.x, second_gate.y);
    }else{
      input_gate = point(x, y);
      output_gate = point(first_gate.x, first_gate.y);
    }
    bool w = true, a = true, s = true, d = true;
    if(output_gate.x == 0){
      w = false;
    }
    if(output_gate.y == 0){
      a = false;
    }
    if(output_gate.x == height_win -1){
      s = false;
    }
    if(output_gate.y == width_win - 1){
      d = false;
    }

    direction = dir;

    if (direction == 1){ //위쪽
      if(w){
        if(wall[output_gate.x - 1][output_gate.y] == 0){
        current_head_x = output_gate.x-1;
        current_head_y = output_gate.y;
      }else{
        current_head_x = search_output(x, y, output_gate).x;
        current_head_y = search_output(x, y, output_gate).y;
      }
      }else{
        current_head_x = search_output(x, y, output_gate).x;
        current_head_y = search_output(x, y, output_gate).y;
      }

    }
    if (direction == 2){ //아래
      if(s){
        if(wall[output_gate.x + 1][output_gate.y] == 0){
        current_head_x = output_gate.x +1;
        current_head_y = output_gate.y;
      }else{

        current_head_x = search_output(x, y, output_gate).x;
        current_head_y = search_output(x, y, output_gate).y;
      }
      }else{

        current_head_x = search_output(x, y, output_gate).x;
        current_head_y = search_output(x, y, output_gate).y;
      }
    }
    if (direction == 3){ //오른쪽
      if(d){
        if(wall[output_gate.x ][output_gate.y + 1] == 0){
        current_head_x = output_gate.x;
        current_head_y = output_gate.y + 1;
      }else{
        current_head_x = search_output(x, y, output_gate).x;
        current_head_y = search_output(x, y, output_gate).y;
      }
      }else{
        current_head_x = search_output(x, y, output_gate).x;
        current_head_y = search_output(x, y, output_gate).y;
      }
    }
    if (direction == 4){ //왼쪽
      if(a){
        if(wall[output_gate.x][output_gate.y - 1] == 0){
        current_head_x = output_gate.x;
        current_head_y = output_gate.y - 1;
      }else{
        current_head_x = search_output(x, y, output_gate).x;
        current_head_y = search_output(x, y, output_gate).y;
      }
      }else{
        current_head_x = search_output(x, y, output_gate).x;
        current_head_y = search_output(x, y, output_gate).y;
      }
    }


  }
