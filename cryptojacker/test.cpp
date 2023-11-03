// 게임 함수를 실행하기 위한 테스트 코드입니다.
// 참고만하셔도 좋습니다.
// ui는 생각하지 않았습니다.
// 지금은 스테이지가 이어서 시작됩니다.

#include "game.hpp"
#include "playerInfo.hpp"
#include "ui.hpp"
#include <algorithm>
#include <fcntl.h>
#include <iostream>
#include <list>
#include <locale.h>
#include <ncurses.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <vector>

using namespace std;
#define ENTER 10 // enter입력값으로 사용
#define STAGE 6  //스테이지의 개수

// numberOfObstacle() * 100
// score 값을 이 값으로 저장해주세요
//점수(장애물 피한 횟수 * 100)
int score;

int main(void) {

    setlocale(LC_ALL, "ko_KR.utf8");
    setlocale(LC_CTYPE, "ko_KR.utf8");
    int time = 1000000;
    int cnt = 0;

    //커서모드 시작
    initscr();

    // init 기본모드 설정
    noecho();
    cbreak();

    //특수키 사용 (방향키 등)
    keypad(stdscr, TRUE);

    //윈도우 테두리 시작위치
    int start_y = 0, start_x = 0;

    // while문 입력확인에 사용
    int ch;

    // 커서의 위치 확인
    int position = 1;

    setWindow1(UIHEIGHT, UIWIDTH, start_x, start_y);

    // 키 입력시까지 반복
    while ((ch = getch()) != KEY_F(8)) {
        switch (ch) {
        case ENTER:
            if (position == 1) {
                // GAME START 출력
                clear();
                Character character = Character();
                vector<Obstacle> obstacle;
                int time = 1000000;
                int cnt = 0;
                Obstacle::total = 0;
                for (int i = 0; i < STAGE; i++) {
                    cnt++;
                    if (cnt == STAGE) {
                        if (game(time, character, obstacle) != 0) {
                            //스테이지를 모두 클리어하는 경우
                            score = 60;
                            setWindow3("CLEAR!", 7, 30, 26);
                            refresh();
                            int score = numberOfObstacle() * 100;
                            mvprintw(UIHEIGHT / 2 + 1,
                                     (UIWIDTH - intlen(score)) / 2, "%d",
                                     score);
                            refresh();
                            fileWrite(score);
                            clear();
                            setWindow1(UIHEIGHT, UIWIDTH, start_x, start_y);
                            refresh();
                            time -= 10000; // 속도 증가 적당히 설정했습니다.
                        } else
                            break;
                    }

                    //장애물에 충돌하지 않으면 게임 실행
                    else if (game(time, character, obstacle) != 0) {
                        initscr();
                        setWindow3("next..", 4, 14, 34);
                        refresh();
                        sleep(1);
                        endwin();
                        time -= 10000; // 속도 증가 적당히 설정했습니다.
                    }
                    //장애물에 충돌하여 게임이 종료되는 경우
                    else {
                        setWindow3("fail..", 7, 30, 26);
                        int score = numberOfObstacle() * 100;
                        mvprintw(UIHEIGHT / 2 + 1,
                                 (UIWIDTH - intlen(score)) / 2, "%d", score);
                        refresh();
                        fileWrite(score);
                        clear();
                        setWindow1(UIHEIGHT, UIWIDTH, start_x, start_y);
                        refresh();
                        break;
                    }
                }
                break;
                // endwin();
                // return 0;
            } else if (position == 2) {
                explain();
                // GAME EXPLAIN 출력
                while (getch() != ENTER) {
                    sleep(0);
                }
                curs_set(2);
                setWindow1(UIHEIGHT, UIWIDTH, start_x, start_y);
                print_explain();
                refresh();
                break;
            } else if (position == 3) {
                // RANKING 출력
                clear();
                setWindow2(UIHEIGHT, UIWIDTH, start_x, start_y);
                fileRead();
                while (getch() != ENTER) {
                    sleep(0);
                }
                clear();
                curs_set(2);
                setWindow1(UIHEIGHT, UIWIDTH, start_x, start_y);
                print_rank();
                refresh();
                break;
            } else if (position == 4) {
                //프로그램 종료
                endwin();
                return 0;
            }
        case KEY_DOWN:
            if (position == 1) {
                ++position;
                print_explain();
                break;
            } else if (position == 2) {
                ++position;
                print_rank();
                break;
            } else if (position == 3) {
                ++position;
                print_exit();
                break;
            } else if (position == 4) {
                position = 1;
                print_start();
                break;
            }
        case KEY_UP:
            if (position == 1) {
                position = 4;
                print_exit();
                break;
            } else if (position == 2) {
                --position;
                print_start();
                break;
            } else if (position == 3) {
                --position;
                print_explain();
                break;
            } else if (position == 4) {
                --position;
                print_rank();
                break;
            }
        }
    }

    refresh();

    //커서모드 종료
    endwin();
    return 0;
}
