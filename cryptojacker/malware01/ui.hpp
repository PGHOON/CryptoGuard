#ifndef __UI_H__
#define __UI_H__
#include <ncurses.h>

#define ENTER 10
#define UIWIDTH 82
#define UIHEIGHT 25

//초기화면 기본설정
void setWindow1(int height, int width, int x, int y);
//높이, 너비, 시작위치(x, y)를 입력받아서 테두리 형성
void setWindow2(int height, int width, int x, int y);
//게임진행 문구 출력
void setWindow3(const char *text, int height, int width, int start_x);

//점수 출력 위치를 위한 int길이
int intlen(int num);

//초기화면 커서 관련
void print_start();
void print_explain();
void print_rank();
void print_exit();

#endif
