#include "game.hpp"
#include "playerInfo.hpp"
#include "ui.hpp"
#include <array>
#include <iostream>
#include <list>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <vector>
using namespace std;

array<array<int, WIDTH>, HEIGHT> Board = {0};
int Obstacle::total = 0;

// game 설명창
void explain() {
    curs_set(0);

    //특수키 사용 (방향키 등)
    keypad(stdscr, TRUE);
    //윈도우 테두리 시작위치
    int start_y = 0, start_x = 0;

    // 커서의 위치 확인
    int position = 1;

    setWindow2(UIHEIGHT, UIWIDTH, 0, 0);

    mvprintw(UIHEIGHT / 2 - 5,
             (UIWIDTH - strlen("1. ⇦ ⇨ 로 캐릭터를 조작하세요.")) / 2 + 7,
             "1. ⇦ ⇨ 로 캐릭터를 조작하세요.");
    mvprintw(
        UIHEIGHT / 2 - 3,
        (UIWIDTH - strlen("2. 다음 조작 전, 캐릭터가 움직이기를 기다리세요.")) /
                2 +
            10,
        "2. 다음 조작 전, 캐릭터가 움직이기를 기다리세요.");
    mvprintw(
        UIHEIGHT / 2 - 1,
        (UIWIDTH -
         strlen("3. 게임이 시작하면, 'SCORE','STAGE'가 상단에 표시됩니다.")) /
                2 +
            8,
        "3. 게임이 시작하면, 'SCORE','STAGE'가 상단에 표시됩니다.");
    mvprintw(UIHEIGHT / 2 + 1,
             (UIWIDTH - strlen("4. 장애물 10개당 1 STAGE씩 증가합니다.")) / 2 +
                 8,
             "4. 장애물 10개당 1 STAGE씩 증가합니다.");
    mvprintw(
        UIHEIGHT / 2 + 3,
        (UIWIDTH - strlen("5. 총 STAGE는 6개, CREAR 점수는 6000점입니다.")) /
                2 +
            6,
        "5. 총 STAGE는 6개, CREAR 점수는 6000점입니다.");
    mvprintw(
        UIHEIGHT / 2 + 5,
        (UIWIDTH - strlen("6. 게임 종료 후 RANKING을 확인해보세요:)")) / 2 + 7,
        "6. 게임 종료 후 RANKING을 확인해보세요:)");

    mvprintw(UIHEIGHT - 1, (UIWIDTH - strlen("[Press Enter]")) / 2,
             "[Press Enter]");

    refresh();
    // 엔터쳐서 돌아가기
}

//게임 시작 함수
int game(int time, Character &character, vector<Obstacle> &obstacle) {
    int score = 0;
    score = numberOfObstacle() * 100;
    int input;
    Board = {0};
    //화면 시작
    initscr();
    cbreak();
    noecho();
    scrollok(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    //초기 화면
    uiBox();
    showState(score, time);
    character.showD();

    //다음 스테이지로 넘어가는 경우 3초 대기 후 시작
    if (numberOfObstacle() != 0) {
        showObstacle(obstacle);
        mvprintw(2, 1, "Game Start After 3 seconds ");
        printw("1.. ");
        refresh();
        sleep(1);
        printw("2.. ");
        refresh();
        sleep(1);
        printw("3.. ");
        refresh();
        sleep(1);
        mvprintw(2, 1, "                                        ");
    }

    //게임 시작
    while (true) {
        //장애물 생성
        generateObstacle(obstacle);

        //게임의 상태 출력 (점수, 스테이지)
        score = numberOfObstacle() * 100;
        showState(score, time);
        usleep(time);

        input = getch();
        // q로 종료
        if (input == 'q') {
            break;
        }
        //오른쪽 이동
        if (input == KEY_RIGHT) {
            character.moveR();
        }
        //왼쪽 이동
        else if (input == KEY_LEFT) {
            character.moveL();
        } else {
            character.showD();
        }

        //충돌한 경우, 게임 종료
        if (crash(character.getY(), character.getX())) {
            break;
        }

        //스테이지 1개 종료
        //지나간 장애물의 수를 반환
        if (numberOfObstacle() % MAXOB == 0 && numberOfObstacle() != 0) {
            endwin();
            return numberOfObstacle();
        }
    }
    endwin();
    return 0;
}

//캐릭터 클래스 구현부분
//생성자
Character::Character() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            Board[location[0] + i][location[1] + j] = 2;
        }
    }
}

//오른쪽 이동
void Character::moveR() {
    if (location[1] >= WIDTH + 2) {
        return;
    }
    //이전 기록 지움
    for (int i = 0; i < motionR.size(); i++) {
        for (int j = 0; j < motionR[i].size(); j++) {
            mvprintw(location[0] + i, location[1] + j, " ", motionR[i][j]);
            Board[location[0] + i][location[1] + j] = 0;
        }
    }

    //움직인 좌표 저장&출력
    location[1] += 1;
    for (int i = 0; i < motionR.size(); i++) {
        for (int j = 0; j < motionR[i].size(); j++) {
            mvprintw(location[0] + i, location[1] + j, "%c", motionR[i][j]);
            //캐릭터가 있는 위치에는 2을 더함
            Board[location[0] + i][location[1] + j] += 2;
        }
    }
    refresh();
}

//왼쪽으로 이동
void Character::moveL() {
    if (location[1] <= 0) {
        return;
    }
    //이전 기록 지움
    for (int i = 0; i < motionL.size(); i++) {
        for (int j = 0; j < motionL[i].size(); j++) {
            mvprintw(location[0] + i, location[1] + j, " ", motionL[i][j]);
            Board[location[0] + i][location[1] + j] = 0;
        }
    }

    //움직인 좌표 저장&출력
    location[1] -= 1;
    for (int i = 0; i < motionL.size(); i++) {
        for (int j = 0; j < motionL[i].size(); j++) {
            mvprintw(location[0] + i, location[1] + j, "%c", motionL[i][j]);
            //캐릭터가 있는 위치에는 2을 더함
            Board[location[0] + i][location[1] + j] += 2;
        }
    }
    refresh();
}

//정지 모습
void Character::showD() {
    for (int i = 0; i < motionD.size(); i++) {
        for (int j = 0; j < motionD[i].size(); j++) {
            mvprintw(location[0] + i, location[1] + j, "%c", motionD[i][j]);
        }
    }
    refresh();
}

//장애물 클래스 구현부분
//생성자
Obstacle::Obstacle() {
    total += 1; //생성된 장애물의 수 증가

    srand(time(NULL));
    length = rand() % MAX_LENGTH + 1; //최대 길이 이하로 장애물 생성

    // TOPX~width-1-length 사이의 랜덤한 위치 설정
    location[1] = rand() % (WIDTH - length) + TOPX;

    //해당 위치에 장애물 저장
    for (int i = 0; i < length; i++) {
        Board[0][location[1] + i] = 1;
    }
}

//장애물 출력
void Obstacle::show() {
    for (int i = 0; i < length; i++) {
        mvprintw(location[0], location[1] + i, "@");
    }
    refresh();
}

//장애물 이동
void Obstacle::moveO() {
    //장애물의 위치가 화면의 길이를 넘어서는 경우
    if (location[0] >= HEIGHT) {
        return;
    } else if (location[0] == HEIGHT - 1) {
        Board[location[0]].fill(0);

        move(location[0], location[1]);
        for (int i = 0; i < length; i++) {
            printw(" ");
        }
        location[0] += 1;
        refresh();
        return;
    }
    //이전 위치 지우기
    move(location[0], location[1]);
    for (int i = 0; i < length; i++) {
        Board[location[0]][location[1] + i] = 0;
        printw(" ");
    }
    //아래로 이동
    location[0] += 1;
    move(location[0], location[1]);
    for (int i = 0; i < length; i++) {
        //장애물이 있는 위치에는 1을 더함
        Board[location[0]][location[1] + i] += 1;
        printw("@");
    }
    refresh();
}

//장애물 생성
void generateObstacle(vector<Obstacle> &obstacle) {
    //장애물의 수가 화면의 길이보다 큰 경우 첫번째 요소를 배열에서 삭제
    if (obstacle.size() > HEIGHT - TOPY) {
        obstacle.erase(obstacle.begin());
    }
    //전체 장애물을 이동
    for (int i = 0; i < obstacle.size(); i++) {
        obstacle[i].moveO();
        mvprintw(WIDTH + 3, i, "%d", i);
    }
    //새로운 장애물을 추가하고 출력
    obstacle.push_back(Obstacle());
    obstacle[obstacle.size() - 1].show();
}

//충돌 판정 함수
//캐릭터의 위치를 받고 캐릭터와 장애물이 겹쳐 있으면 true 반환
bool crash(int y, int x) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (Board[y + i][x + j] >= 3) {
                return true;
            }
        }
    }
    return false;
}

//점수와 스테이지 출력
void showState(int score, int time) {
    int stage = (1000000 - time) / 10000 + 1;
    mvprintw(1, 1, "score: %d stage %d", score, stage);
    refresh();
}

//회피한 장애물의 수 반환
int numberOfObstacle() {
    int total = Obstacle::total;
    if (total < HEIGHT - TOPY + 1) {
        return 0;
    }
    return total - HEIGHT + TOPY; //전체 생성된 장애물의 수 - 화면 크기
}

//전체 장애물 출력
void showObstacle(vector<Obstacle> &obstacle) {
    for (int i = 1; i < obstacle.size(); i++) {
        obstacle[i].show();
    }
}

// 게임 화면 ui
void uiBox() {
    setWindow2(HEIGHT - 2, WIDTH + 2, 0, 3);
    setWindow2(4, WIDTH + 2, 0, 0);
}

void uiBox2(WINDOW *my_win) {
    my_win = newwin(HEIGHT - 3, WIDTH + 2, 0, 3);
    refresh();

    box(my_win, 0, 0);

    wrefresh(my_win);
}

// Board 저장값을 출력 (컴파일용)
void showBoard() {
    clear();
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            mvprintw(i, j, "%d", Board[i][j]);
        }
    }
    refresh();
    sleep(2);
}
