#ifndef __GAME_H__
#define __GAME_H__
#include <array>
#include <iostream>
#include <list>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <vector>

using namespace std;

#define TOPY 4 //장애물 출력 위치 y값
#define TOPX 1 //장애물 출력 위치 x값

//$게임 화면 크기 설정
//#define WIDTH 44
//#define HEIGHT 24
#define WIDTH 80
#define HEIGHT 24

//$스테이지의 길이(스테이지당 최대 장애물 수)
#define MAXOB 10

//장애물의 최대 길이
#define MAX_LENGTH 5

// 캐릭터 클래스
// 캐릭터의 위치 저장 및 출력
class Character {

  public:
    Character();
    void showD();
    void moveR();
    void moveL();

    int getX() { return location[1]; }
    int getY() { return location[0]; }

    //$캐릭터 ui
  private:
    //정지
    array<array<char, 3>, 3> motionD = {' ',  'O', ' ', '/', 'T',
                                        '\\', '|', ' ', '|'};
    //오른쪽 이동
    array<array<char, 3>, 3> motionR = {' ',  'O', ' ', '/', 'T',
                                        '\\', '|', ' ', '\\'};
    //왼쪽 이동
    array<array<char, 3>, 3> motionL = {' ',  'O', ' ', '/', 'T',
                                        '\\', '/', ' ', '|'};
    //위치 저장
    array<int, 2> location = {HEIGHT - 3, (int)(WIDTH - 3) / 2 + TOPX};
};

//장애물 클래스
//장애물 아이콘 '@'
class Obstacle {

  public:
    static int total; //총 생성된 장애물의 수

    //장애물 생성
    Obstacle();
    //장애물 출력 및 이동
    void show();
    void moveO();

  private:
    int length;                            //장애물의 길이
    array<int, 2> location = {TOPY, TOPX}; //장애물의 위치
};

//게임 설명 화면
void explain();

//회피한 장애물의 갯수를 반환
int numberOfObstacle();

//게임 시작 함수
//충돌할 경우 0반환
//최대 장애물의 수를 회피한 경우 회피한 장애물의 수 반환
int game(int time, Character &character, vector<Obstacle> &obstacle);

//장애물을 발생시키는 함수
void generateObstacle(vector<Obstacle> &obstacle);

//전체 장애물을 보여주는 함수
void showObstacle(vector<Obstacle> &obstacle);

//충돌 판단 함수
bool crash(int y, int x);

//게임의 상태를 나타냄, 스테이지 정보, 점수
void showState(int score, int time);

//게임 화면 ui
void uiBox();
void uiBox2(WINDOW *my_win);

//컴파일용
void showBoard();
#endif
