#include <ncurses.h>
#include <iostream>
#include <locale.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <ctime>
#include <random>
#include <cstdlib>
#include <queue>
#include <vector>
#include <string>

using namespace std;

int pre = 4;
struct winsize w;
WINDOW *win1;

class point{
public:
  int x;
  int y;
  point(int num1=0, int num2=0){
    x=num1;
    y=num2;
  }
  point(const point& pt){
    x=pt.x;
    y=pt.y;
  }

  point& operator=(const point& pt){
    this->x = pt.x;
    this->y = pt.y;
    return *this;
  }

  bool operator==(const point& pt) const{
    return (x==pt.x)&&(y==pt.y);
  }
  bool operator!=(const point& pt) const{
    return !(*this==pt);
  }
  bool isInclude(int len, const vector<point> pt) const{
   for (int i=0;i<len;i++){
     if (*this == pt[i]){
       return true;
     }
   }
   return false;
 }
  friend ostream& operator<<(ostream &os, const point& pt){
    os << "(" << pt.x << "," << pt.y << ")";
    return os;
  }
};

class SnakeGame{
private:
  point first_gate;// 첫번째 게이트의 좌표
  point second_gate;// 두번째 게이트의 좌표
  int size_map;// map의 한 변의 길이(기본 21x21, 정사각형)
  int len_wall = 3;// map 내부에 생성되는 wall의 길이(기본 3)
  int count_wall = 3;// map 내부에 생성되는 wall의 개수(기본 3)
  int stage = 1;// 스테이지 수(현재 stage 레벨)
  bool len_mission = false;
  bool grow_mission = false;
  bool poison_mission = false;
  bool gate_mission = false;//미션 클리어 여부 검사;




  int height = w.ws_row;
  int width = w.ws_col;
  int height_win = height-10;
  int width_win = width/2;
  int **wall;

  int snake_speed = 500*1000;// sec: 뱀 속도
  int num_growth = 0;// growth item을 먹은 개수
  int num_poison = 0;// poison item을 먹은 개수
  int num_gate = 0;// gate를 통과한 횟수
  char play_time[3];// 종료 시간 - 시작시간. seconds로 계산한다.
  int direction = 1; // 키보드 받아서 처리할 예정

  int current_head_x;// 뱀 머리
  int current_head_y;
  int latest_x;
  int latest_y;
  int tail_x;
  int tail_y;
  int next_tail_x;
  int next_tail_y;
  int len_snake = 4;// 뱀 길이
  vector<point> snakep;

  queue<point> growths; // growth_item point  모음
  queue<point> poisons; // poison_item point  모음
  int randomgx=0;
  int randomgy=0;
  int randompx=0;
  int randompy=0;
  // vector<int> sna(len_snake);

public:
  bool stage_clear = false;
  bool started = true;
  SnakeGame(){
    make_stage();

    //snakep = new point[len_snake];


    if(wall[height_win/2][width/4] == 1 || wall[height_win/2][width/4 + 1] == 1// 벽생성된 곳에 뱀이 생성되지 않도록
      ||wall[height_win/2][width/4 + 2] == 1 || wall[height_win/2][width/4 + 3] == 1){
        int i = 0;
        while(wall[height_win/2][width/4 - i] == 1 || wall[height_win/2][width/4 + 1 - i] == 1
          ||wall[height_win/2][width/4 + 2- i] == 1 || wall[height_win/2][width/4 + 3 - i] == 1){
            i++;
          }

          snakep.push_back(point(height_win/2,width/4-i));
          snakep.push_back(point(height_win/2, width/4+1-i));
          snakep.push_back(point(height_win/2, width/4+2-i));
          snakep.push_back(point(height_win/2, width/4+3-i));

        }
        else{
          snakep.push_back(point(height_win/2,width/4));
          snakep.push_back(point(height_win/2, width/4+1));
          snakep.push_back(point(height_win/2, width/4+2));
          snakep.push_back(point(height_win/2, width/4+3));

        }
        current_head_x = snakep[0].x;
        current_head_y = snakep[0].y;
        tail_x = snakep[len_snake-1].x; //row
        tail_y = snakep[len_snake-1].x;


        for (int i=0;i<len_snake;i++){
          if (i==0){
            mvwprintw(win1, snakep[i].x, snakep[i].y, "3");
          }
          else{
            mvwprintw(win1, snakep[i].x,snakep[i].y,"4");
          }
        }

        wrefresh(win1);
  }//end constructor

  void snake_move(int dir){
    int cx = snakep[0].x;
    int cy = snakep[0].y;
    if(wall[cx][cy] == 9){
      meet_gate(cx, cy, dir);
    }else{

    direction = dir;

    if (direction == 1){ //위쪽
      current_head_x = current_head_x-1;
      current_head_y = current_head_y;
    }
    if (direction == 2){ //아래
      current_head_x = current_head_x+1;
      current_head_y = current_head_y;
    }
    if (direction == 3){ //오른쪽
      current_head_x = current_head_x;
      current_head_y = current_head_y+1;
    }
    if (direction == 4){ //왼쪽
      current_head_x = current_head_x;
      current_head_y = current_head_y-1;
    }
  }

    point *tmp = new point[len_snake-1];
    tail_x = snakep[len_snake-1].x;
    tail_y = snakep[len_snake-1].y;

    for(int i=0; i<len_snake-1; i++){
        tmp[i] = snakep[i];
    }

    snakep[0] = point(current_head_x, current_head_y);

    for(int i=1; i<len_snake; i++){
        snakep[i] = tmp[i-1];
    }

    len_snake = snakep.size();

    for (int i=0;i<len_snake;i++){
      if (wall[snakep[i].x][snakep[i].y]==0){
        if (i==0){
          mvwprintw(win1,snakep[i].x,snakep[i].y,"3");
        }
        else{
          mvwprintw(win1,snakep[i].x,snakep[i].y,"4");
        }
      }
    }

    // for (int i=0;i<len_snake;i++){
    //   wall[snakep[i].x][snakep[i].y]=4;
    // }  //이거 넣으면 while문에서 호출될 때마다 3이 늘어남.
    if(wall[tail_x][tail_y] != 9){
      mvwprintw(win1,tail_x,tail_y,"0");
    }


    wrefresh(win1);
  }// end snake move

  void make_stage(){

    wall = new int*[height_win];
    for (int i=0;i<height_win;i++){
      wall[i] = new int[width_win];
    }//벽 크기 생성(화면 높이 * 화면 너비)

    for(int i=0; i<height_win; i++){
      for(int j = 0; j<width_win; j++){
        if((i == 0 && (j == 0|| j == width_win -1))
        ||(i == height_win -1 &&(j == 0 || j == width_win -1))){
          wall[i][j] = 2;//4개의 꼭짓점은 2
        }
        else if(i == 0 || i == height_win - 1){
          wall[i][j] = 1;
        }
        else if(j == 0|| j == width_win -1){
          wall[i][j] = 1;
        }
        else{
          wall[i][j]=0;
        }
      }
    }//wall 값

    for(int i = 0; i<height_win; i++){
      for(int j = 0; j<width_win; j++){
        if(wall[i][j] == 2){
          mvwprintw(win1, i, j, "2");
        }
        else if(wall[i][j] == 1){
          mvwprintw(win1, i, j, "1");
        }
        else{
          mvwprintw(win1, i, j, "0");
        }
      }
    }//화면에 표시
    wrefresh(win1);
    getch();

    int numx;
    int numy;
    srand(static_cast<unsigned int>(time(NULL)));
    int plus =1;
    for (int i=0;i<count_wall;++i){
      int num1 = rand();
      int random1 = (int)num1%height_win; // num1은 0~width_win-1까지의 난수
      int num2 = rand();
      int random2 = (int)num2%width_win;

      if(random1 <= height_win - (len_wall + 2) && 2 <= random1 && random1 != height_win/2
        &&random2 <= width_win  - (len_wall + 2) && 2 <= random2
        &&wall_include(random1, random2)){
          numx = random1; numy = random2;
          for (int k=0;k<count_wall;k++){
            if (wall[numx][numy] == 0){
             for(int h = 0; h < len_wall; h++){
               if(plus < 0){
                 mvwprintw(win1,numx +h,numy,"1");
                 wall[numx+h][numy] = 1;
               }else{
                 mvwprintw(win1,numx,numy+h,"1");
                 wall[numx][numy +h] = 1;
               }

              }
              plus *= -1;
           }

         }
      }else{
        i--;
      }

    }

   //  for (int i=0;i<count_wall;++i){
   //    int num2 = rand();
   //    int random = (int)num2%width_win;// num2는 0~height_win-1까지의 난수
   //    if((random <= width_win  - (len_wall + 2)) && 2 <= random){
   //      random_y[i] = random;
   //    }else{
   //      i--;
   //    }
   //  }
   //
   //  int plus = 1;
   //  for (int k=0;k<count_wall;k++){
   //    int numx = random_x;
   //    int numy = random_y;
   //    if (wall[numy][numx] == 0){
   //     for(int h = 0; h < len_wall; h++){
   //       if(plus <0){
   //         mvwprintw(win1,numy +h,numx,"1");
   //         wall[numy+h][numx] = 1;
   //       }else{
   //         mvwprintw(win1,numy,numx+h,"1");
   //         wall[numy][numx+h] = 1;
   //       }
   //
   //      }
   //   }
   //   plus *= -1;
   // }


   //immune wall 십자벽 보고싶을 때
// int plus = 1;
//   int numx = width/4;
//   int numy = height_win/2;
//   if (wall[numy][numx] == 0){
//    for(int h = 0; h < len_wall; h++){
//        mvwprintw(win1,numy -h,numx,"1");
//        mvwprintw(win1,numy +h,numx,"1");
//        wall[numy+h][numx] = 1;
//        wall[numy-h][numx] = 1;
//        mvwprintw(win1,numy,numx+h,"1");
//        mvwprintw(win1,numy,numx-h,"1");
//        wall[numy][numx-h] = 1;
//        wall[numy][numx+h] = 1;
//      }
//   }


   //교차하는 벽생성시 교차부분은 ImmuneWall로 바꿈///
    for(int i = 1; i<height_win - 1; i++){
      for(int j = 1; j<width_win - 1; j++){
        if(wall[i][j] == 1){
          if(wall[i - 1][j] == 1 &&wall[i][j - 1] == 1
            && wall[i][j + 1] == 1 && wall[i + 1][j] == 1){
              mvwprintw(win1,i, j,"2");
              wall[i][j] == 2;
          }
        }
      }
    }
    wrefresh(win1);
  }

  bool wall_include(int x, int y){
    for(int i = 0; i < len_wall; i++){
      if(wall[x + i][y] == 1){
        return false;
      }
      if(wall[x][y + i] == 1){
        return false;
      }
    }
    return true;
  }




  void make_item(){//growth item,  poison item은 wall에
    srand(static_cast<unsigned int>(time(NULL)));
    //growth item
    int num[4]={0,};

    point growth = point();
    point poison= point();

    while(growth==poison || wall[growth.x][growth.y]!=0 ||
    wall[poison.x][poison.y]!=0 || growth.isInclude(len_snake,snakep)
  ||poison.isInclude(len_snake,snakep)){
      for (int i=0;i<4;i++){
        num[i] = rand();
        if (i==0){
          growth.x = (int)num[i]%height_win;
        }
        else if (i==1){
          growth.y = (int)num[i]%width_win;
        }
        else if (i==2){
          poison.x = (int)num[i]%height_win;
        }
        else{
          poison.y = (int)num[i]%width_win;
        }
      }
    }

    wall[growth.x][growth.y] = 5;//growth
    wall[poison.x][poison.y] = 6; //poison
    mvwprintw(win1,growth.x,growth.y,"A");
    mvwprintw(win1,poison.x,poison.y,"X");
    wrefresh(win1);

    growths.push(growth);
    poisons.push(poison);

  }

  void delete_item(){
    point growth = growths.front();
    point poison = poisons.front();

    if (growth.x!=0  && growth.y!=0 && poison.x!=0 && poison.y!=0){
      wall[growth.x][growth.y]=0;
      wall[poison.x][poison.y]=0;
      mvwprintw(win1, growth.x,growth.y,"0");
      mvwprintw(win1,poison.x,poison.y,"0");
    }

    growths.pop();
    poisons.pop();

    wrefresh(win1);
  }

  void next_stage(){} //미션 생성 + 벽 새로 만들기

  void new_gate(){//게이트 생성(wall의 9가 게이트)
    int count = 0;
    for(int i = 0; i<height_win; i++){
      for(int j = 0; j<width_win; j++){
        if(wall[i][j] == 1){
          count++;
        }
      }
    }
    srand(static_cast<unsigned int>(time(NULL)));
    int random1 = (int)rand()%count;

    int check = 0;
    for(int i = 0; i<height_win; i++){//첫번째 게이트 생성
      if(check == -1){
        break;
      }
      for(int j = 0; j<width_win; j++){
        if(wall[i][j] == 1 && check == random1){
          first_gate = point(i, j);
          wall[i][j] = 9;
          mvwprintw(win1,i, j,"9");
          check = -1;
          break;
        }else if(wall[i][j] == 1){
          check++;
        }
    }
  }

    int random2 = (int)rand()%count;
    while(random1 ==random2){
      random2 = (int)rand()%count;
    }
    check = 0;
    for(int i = 0; i<height_win; i++){//두번째 게이트 생성
      if(check == -1){
        break;
      }
      for(int j = 0; j<width_win; j++){
        if(wall[i][j] == 1 && check == random2){
          second_gate = point(i, j);
          wall[i][j] = 9;
          mvwprintw(win1,i, j,"9");
          check = -1;
          break;
        }else if(wall[i][j] == 1){
          check++;
        }
    }
  }
}

 //게이트생성


  void set_speed(double sec){
    snake_speed = sec*1000*1000;
  }

  int get_speed(){
    return snake_speed;
  }

  void poison_item(){
    num_poison++;
  }
  void growth_item(){
    num_growth++;
  }

  void mission_show(WINDOW *w){
    //len_mission, grow_mission, poison_mission, gate_mission bool타입 추가해주세욤 초기false;
    mvwprintw(w,1,1,"Clear");mvwprintw(w,1,15,"End");
    wborder(w,'|','|','-','-','|','|','|','|');
    char len[3];
    char grow[3];
    char poison[3];
    char gate[3];

    sprintf(len,"%d", len_snake);
    sprintf(grow,"%d", num_growth);
    sprintf(poison,"%d", num_poison);
    sprintf(gate, "%d", num_gate);



    mvwprintw(w,2,1,"B: "); mvwprintw(w, 2, 5, len);
    mvwprintw(w,3,1,"+: "); mvwprintw(w, 3, 5, grow);
    mvwprintw(w,4,1,"G: "); mvwprintw(w, 4, 5, gate);

    mvwprintw(w,2,15,"-: "); mvwprintw(w, 2, 19, poison);

    switch (stage) {
      case 1://뱀 몸 길이 10이상, growth_item 6이상, poison_item 5 이하, gate 5 이상
        //clear mission
        mvwprintw(w, 2, 7, "/10");
        if(len_snake >= 10){
          mvwprintw(w, 2, 10, "(V)");
          len_mission = true;
        }else{mvwprintw(w, 2, 10, "( )");}

        mvwprintw(w, 3, 7, "/6");
        if(num_growth >= 6){
          mvwprintw(w, 3, 10, "(V)");
          grow_mission = true;
        }else{mvwprintw(w, 3, 10, "( )");}

        mvwprintw(w, 4, 7, "/5");
        if(num_gate >= 5){
          mvwprintw(w, 4, 10, "(V)");
          gate_mission = true;
        }else{mvwprintw(w, 4, 10, "( )");}

        //end mission(true가 되면 게임 오버)
        mvwprintw(w, 2, 21, "/5");
        if(num_poison >= 5){
          mvwprintw(w, 2, 24, "(V)");
          poison_mission = true;
        }else{mvwprintw(w, 2, 24, "( )");}
        break;
      case 2://뱀 몸 길이 15이상, growth_item 10이상, poison_item 4 이하, gate 10 이상
      //clear mission
      mvwprintw(w, 2, 7, "/15");
      if(len_snake >= 15){
        mvwprintw(w, 2, 10, "(V)");
        len_mission = true;
      }else{mvwprintw(w, 2, 10, "( )");}

      mvwprintw(w, 3, 7, "/10");
      if(num_growth >= 10){
        mvwprintw(w, 3, 10, "(V)");
        grow_mission = true;
      }else{mvwprintw(w, 3, 10, "( )");}

      mvwprintw(w, 4, 7, "/10");
      if(num_gate >= 10){
        mvwprintw(w, 4, 10, "(V)");
        gate_mission = true;
      }else{mvwprintw(w, 4, 10, "( )");}

      //end mission(true가 되면 게임 오버)
      mvwprintw(w, 2, 21, "/4");
      if(num_poison >= 4){
        mvwprintw(w, 2, 24, "(V)");
        poison_mission = true;
      }else{mvwprintw(w, 2, 24, "( )");}
      break;

      case 3://뱀 몸 길이 20이상, growth_item 10이상, poison_item 3 이하, gate 20 이상
      //clear mission
      mvwprintw(w, 2, 7, "/20");
      if(len_snake >= 20){
        mvwprintw(w, 2, 10, "(V)");
        len_mission = true;
      }else{mvwprintw(w, 2, 10, "( )");}

      mvwprintw(w, 3, 7, "/10");
      if(num_growth >= 10){
        mvwprintw(w, 3, 10, "(V)");
        grow_mission = true;
      }else{mvwprintw(w, 3, 10, "( )");}

      mvwprintw(w, 4, 7, "/20");
      if(num_gate >= 20){
        mvwprintw(w, 4, 10, "(V)");
        gate_mission = true;
      }else{mvwprintw(w, 4, 10, "( )");}

      //end mission(true가 되면 게임 오버)
      mvwprintw(w, 2, 21, "/3");
      if(num_poison >= 3){
        mvwprintw(w, 2, 24, "(V)");
        poison_mission = true;
      }else{mvwprintw(w, 2, 24, "( )");}
      break;

      case 4://뱀 몸 길이 25이상, growth_item 15이상, poison_item 3 이하, gate 30 이상
      //clear mission
      mvwprintw(w, 2, 7, "/25");
      if(len_snake >= 25){
        mvwprintw(w, 2, 10, "(V)");
        len_mission = true;
      }else{mvwprintw(w, 2, 10, "( )");}

      mvwprintw(w, 3, 7, "/15");
      if(num_growth >= 15){
        mvwprintw(w, 3, 10, "(V)");
        grow_mission = true;
      }else{mvwprintw(w, 3, 10, "( )");}

      mvwprintw(w, 4, 7, "/30");
      if(num_gate >= 30){
        mvwprintw(w, 4, 10, "(V)");
        gate_mission = true;
      }else{mvwprintw(w, 4, 10, "( )");}

      //end mission(true가 되면 게임 오버)
      mvwprintw(w, 2, 21, "/3");
      if(num_poison >= 3){
        mvwprintw(w, 2, 24, "(V)");
        poison_mission = true;
      }else{mvwprintw(w, 2, 24, "( )");}
      break;

    }

  }
  void score_show(WINDOW *w){////////////////////////////////////
    // point *pt2 = new point(3);
    //  pt2[0] = point(3,3);
    //  point pt3(3,3);
    //  bool ans = pt3.isInclude(3,pt2);
    //  if (ans == 1){
    //    mvwprintw(w,4,4,"O");
    //  }
    //  else{
    //    mvwprintw(w,4,4,"X");
    //  }
     mvwprintw(w, 3, 3, play_time);
   }

  void score_final(){

  }

  void goHead(int dir, int x, int y, int c=0){
    if (c==0){

      if (dir == 1){ //위쪽
        current_head_x = x-1;
      }
      if (dir == 2){ //아래
        current_head_x = x+1;
      }
      if (dir == 3){ //오른쪽
        current_head_y = y+1;
      }
      if (dir == 4){ //왼쪽
        current_head_y = y-1;
      }
    }
    else{
      current_head_x = x;
      current_head_y = y;
    }
  }


  point search_output(int x, int y, point output_gate){
      bool w = true, a = true, s = true, d = true;
      //w = 1, a = 4, s = 2, d = 3
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
      int number = 0;
      string sequence[4] = {"1342", "2431", "3214", "4123"};
      for(int i = 0; i < 4; i++){
        if(sequence[i].at(0) == (char)(pre + 48)){
          number = i;
          break;
        }
      }
      for(int i = 0; i < 4; i++){
        switch(sequence[number].at(i)){
          case '1':
            if(w && wall[output_gate.x - 1][output_gate.y] == 0){
              pre = 1;
              return point(output_gate.x-1, output_gate.y);
            }
            break;
          case '2':
            if(s && wall[output_gate.x + 1][output_gate.y] == 0){
              pre = 2;
              return point(output_gate.x + 1, output_gate.y);
            }
            break;
          case '3':
            if(d && wall[output_gate.x][output_gate.y +1] == 0){
              pre = 3;
              return point(output_gate.x, output_gate.y +1);
            }
            break;
          case '4':
            if(a && wall[output_gate.x][output_gate.y - 1] == 0){
              pre = 4;
              return point(output_gate.x, output_gate.y - 1);
            }
            break;
        }
      }

    }
//   point search_output(int x, int y, point output_gate){
//   bool w = true, a = true, s = true, d = true;
//   if(output_gate.x == 0){
//     w = false;
//   }
//   if(output_gate.y == 0){
//     a = false;
//   }
//   if(output_gate.x == height_win -1){
//     s = false;
//   }
//   if(output_gate.y == width_win - 1){
//     d = false;
//   }
//   if(w){
//     if(wall[output_gate.x - 1][output_gate.y] == 0){
//     pre = 1;
//     return point(output_gate.x-1, output_gate.y);
//     }
//   }
//   if(d){
//     if(wall[output_gate.x][output_gate.y +1] == 0){
//     pre = 3;
//     return point(output_gate.x, output_gate.y +1);
//   }
//   }
//   if(s){
//     if(wall[output_gate.x + 1][output_gate.y] == 0){
//     pre = 2;
//     return point(output_gate.x + 1, output_gate.y);
//   }
//   }
//   if(a){
//     if(wall[output_gate.x][output_gate.y - 1] == 0){
//     pre = 4;
//     return point(output_gate.x, output_gate.y - 1);
//     }
//   }
//
//
// }

// void meet_gate(int x, int y, int dir){
//     point input_gate, output_gate;
//     if(first_gate.x == x && first_gate.y == y){
//       input_gate = point(x, y);
//       output_gate = point(second_gate.x, second_gate.y);
//     }else{
//       input_gate = point(x, y);
//       output_gate = point(first_gate.x, first_gate.y);
//     }
//     bool w = true, a = true, s = true, d = true;
//     if(output_gate.x == 0){
//       w = false;
//     }
//     if(output_gate.y == 0){
//       a = false;
//     }
//     if(output_gate.x == height_win -1){
//       s = false;
//     }
//     if(output_gate.y == width_win - 1){
//       d = false;
//     }
//
//     direction = dir;
//
//     if (direction == 1){ //위쪽
//       if(w){
//         if(wall[output_gate.x - 1][output_gate.y] == 0){
//         current_head_x = output_gate.x-1;
//         current_head_y = output_gate.y;
//       }else{
//         current_head_x = search_output(x, y, output_gate).x;
//         current_head_y = search_output(x, y, output_gate).y;
//       }
//       }else{
//         current_head_x = search_output(x, y, output_gate).x;
//         current_head_y = search_output(x, y, output_gate).y;
//       }
//
//     }
//     if (direction == 2){ //아래
//       if(s){
//         if(wall[output_gate.x + 1][output_gate.y] == 0){
//         current_head_x = output_gate.x +1;
//         current_head_y = output_gate.y;
//       }else{
//
//         current_head_x = search_output(x, y, output_gate).x;
//         current_head_y = search_output(x, y, output_gate).y;
//       }
//       }else{
//
//         current_head_x = search_output(x, y, output_gate).x;
//         current_head_y = search_output(x, y, output_gate).y;
//       }
//     }
//     if (direction == 3){ //오른쪽
//       if(d){
//         if(wall[output_gate.x ][output_gate.y + 1] == 0){
//         current_head_x = output_gate.x;
//         current_head_y = output_gate.y + 1;
//       }else{
//         current_head_x = search_output(x, y, output_gate).x;
//         current_head_y = search_output(x, y, output_gate).y;
//       }
//       }else{
//         current_head_x = search_output(x, y, output_gate).x;
//         current_head_y = search_output(x, y, output_gate).y;
//       }
//     }
//     if (direction == 4){ //왼쪽
//       if(a){
//         if(wall[output_gate.x][output_gate.y - 1] == 0){
//         current_head_x = output_gate.x;
//         current_head_y = output_gate.y - 1;
//       }else{
//         current_head_x = search_output(x, y, output_gate).x;
//         current_head_y = search_output(x, y, output_gate).y;
//       }
//       }else{
//         current_head_x = search_output(x, y, output_gate).x;
//         current_head_y = search_output(x, y, output_gate).y;
//       }
//     }
//
//
//   }


    //
    void meet_gate(int x, int y, int dir){
    num_gate+=1;
    point input_gate, output_gate;
    if(first_gate.x == x && first_gate.y == y){
      input_gate = point(x, y);
      output_gate = point(second_gate.x, second_gate.y);
    }else{
      input_gate = point(x, y);
      output_gate = point(first_gate.x, first_gate.y);
    }
    goHead(dir, search_output(x, y, output_gate).x,search_output(x, y, output_gate).y,1);

    }



  // void meet_gate(int x, int y, int dir){
  //    num_gate++;
  //    point input_gate, output_gate;
  //    if(first_gate.x == x && first_gate.y == y){
  //      input_gate = point(x, y);
  //      output_gate = point(second_gate.x, second_gate.y);
  //    }else{
  //      input_gate = point(x, y);
  //      output_gate = point(first_gate.x, first_gate.y);
  //    }
  //    bool w = true, a = true, s = true, d = true;
  //    if(output_gate.x == 0){ w = false; }
  //    if(output_gate.y == 0){ a = false; }
  //    if(output_gate.x == height_win -1){ s = false; }
  //    if(output_gate.y == width_win - 1){ d = false; }
  //
  //    direction = dir;
  //
  //    if (direction == 1){ //위쪽
  //      if(w){
  //        if(wall[output_gate.x - 1][output_gate.y] == 0){
  //        current_head_x = output_gate.x-1;
  //        current_head_y = output_gate.y;
  //        }
  //       else{
  //          current_head_x = search_output(x, y, output_gate).x;
  //          current_head_y = search_output(x, y, output_gate).y;
  //        }
  //      }
  //      else{
  //        current_head_x = search_output(x, y, output_gate).x;
  //        current_head_y = search_output(x, y, output_gate).y;
  //      }
  //    }
  //    if (direction == 2){ //아래
  //      if(s){
  //        if(wall[output_gate.x + 1][output_gate.y] == 0){
  //        current_head_x = output_gate.x +1;
  //        current_head_y = output_gate.y;
  //      }else{
  //
  //        current_head_x = search_output(x, y, output_gate).x;
  //        current_head_y = search_output(x, y, output_gate).y;
  //      }
  //      }else{
  //
  //        current_head_x = search_output(x, y, output_gate).x;
  //        current_head_y = search_output(x, y, output_gate).y;
  //      }
  //    }
  //    if (direction == 3){ //오른쪽
  //      if(d){
  //        if(wall[output_gate.x ][output_gate.y + 1] == 0){
  //        current_head_x = output_gate.x;
  //        current_head_y = output_gate.y + 1;
  //      }else{
  //        current_head_x = search_output(x, y, output_gate).x;
  //        current_head_y = search_output(x, y, output_gate).y;
  //      }
  //      }else{
  //        current_head_x = search_output(x, y, output_gate).x;
  //        current_head_y = search_output(x, y, output_gate).y;
  //      }
  //    }
  //    if (direction == 4){ //왼쪽
  //      if(a){
  //        if(wall[output_gate.x][output_gate.y - 1] == 0){
  //        current_head_x = output_gate.x;
  //        current_head_y = output_gate.y - 1;
  //      }else{
  //        current_head_x = search_output(x, y, output_gate).x;
  //        current_head_y = search_output(x, y, output_gate).y;
  //      }
  //      }else{
  //        current_head_x = search_output(x, y, output_gate).x;
  //        current_head_y = search_output(x, y, output_gate).y;
  //      }
  //    }
  //
  //
  //  }


  void set_playtime(double r){ //------------------------------
    sprintf(play_time,"%f", r);
  }

  bool get_started(){
    return started;
  }
  void game_end(){//stage_clear, started 검사 !!
    int cx = snakep[0].x;
    int cy = snakep[0].y;
    if(wall[cx][cy] == 1){
      started = false;
    }
    if(len_snake < 3){
      started = false;
    }
    for(int i=1; i<len_snake; i++){
      if (snakep[0]==snakep[i]){
        started = false;
      }
    }
    if(poison_mission == true){
      started = false;
    }

    int mission = 0;
    if(len_mission == true){mission++;}
    if(grow_mission == true){mission++;}
    if(gate_mission == true){mission++;}
    switch (stage){
      case 1:
        if(mission >= 1){stage_clear = true; started = false;}
        break;

      case 2:
        if(mission >= 2){stage_clear = true; started = false;}
        break;
        break;
      case 3:
        if(mission >= 3){stage_clear = true; started = false;}
        break;
      case 4:
        if(mission >= 3){stage_clear = true; started = false;}
        break;
    }

  }

};

int main(){
  setlocale(LC_ALL, "ko_KR.utf8");
  setlocale(LC_CTYPE, "ko_KR.utf8");//한글 출력 설정
  // struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  char key;
  char userName[8];
  WINDOW *win2;
  WINDOW *win3;
  int height = w.ws_row;
  int width = w.ws_col;
  int height_win = w.ws_row-10;
  int width_win = w.ws_col/2;

  initscr();//curse 모드 시작
  keypad(stdscr, TRUE);
  curs_set(0);
  cbreak();
  echo();

  printw("User name: ");
  scanw("%s", userName);

  getch();
  refresh();

  start_color();
  init_pair(1,COLOR_WHITE,COLOR_BLUE);//색 attribute 설정
  init_pair(2,COLOR_BLUE,COLOR_WHITE);
  init_pair(3,COLOR_WHITE,COLOR_RED);

// //숫자, 폰트 색, 폰트
  border('|','|','-','-','|','|','|','|');
  mvprintw(1,1, "SnakeGame");
  // mvprintw(2,1,"User : ");
  // mvprintw(2,2,userName);
  mvprintw(2,1,"User : ");
  printw(userName,"\n");
  refresh();
  getch();
//setlocale하면 한글되나봄.
  win1= newwin(height_win,width_win,3,3);
  wbkgd(win1,COLOR_PAIR(2));
  wattron(win1, COLOR_PAIR(2));
  SnakeGame s1 = SnakeGame();
  wrefresh(win1);
  getch();
  s1.make_item();
  s1.make_item();
  s1.make_item();
  getch();
  s1.new_gate();

  win2 = newwin(height/3, width/3, 3, width/2 + 10);
  wbkgd(win2,COLOR_PAIR(1));
  wattron(win2, COLOR_PAIR(1));
  // mission_show(win2);
  wrefresh(win2);

  win3 = newwin(height/3,width/3, height/2, width/2+10);
  wbkgd(win3,COLOR_PAIR(3));
  wattron(win3, COLOR_PAIR(3));
  s1.mission_show(win3);
  wrefresh(win3);

  // getch();
  refresh();

  nodelay(stdscr, TRUE);
  // int pre = 4;

  //시간측정
  time_t start, end;//------------------------------------------------
  double result, current;
  start = time(NULL);
  time_t first,mid;
  // first = time(NULL);
//  s1.make_item();

  while (s1.get_started()){
    int input;
    // mid = time(NULL);
    // if (mid-first>=1.0){
    //   s1.delete_item();
    //   s1.make_item();
    //   first = time(NULL);
    // }
    input  = getch();
    switch (input){
      case KEY_LEFT:
        s1.snake_move(4);
        break;
      case KEY_RIGHT:
        s1.snake_move(3);
        break;
      case KEY_DOWN:
        s1.snake_move(2);
        break;
      case KEY_UP:
        s1.snake_move(1);
        break;
      case ERR:
        s1.snake_move(pre);
        break;
      }
    if(input == KEY_LEFT){
      pre = 4;
    }else if(input == KEY_RIGHT){
      pre = 3;
    }else if(input == KEY_DOWN){
      pre = 2;
    }else if(input == KEY_UP){
      pre = 1;
    }
    s1.game_end();

    s1.mission_show(win3);
    wrefresh(win3);

    wrefresh(win1);
    usleep(s1.get_speed());
  }
  end = time(NULL);//////////////////////////////////////////////////////
  result = (double)(end - start);
  s1.set_playtime(result);

  nodelay(stdscr, false);

  s1.score_show(win3);
  wrefresh(win3);
  getch();

  delwin(win1);
  delwin(win2);
  delwin(win3);

  endwin();//curses 모드

  return 0;

}
