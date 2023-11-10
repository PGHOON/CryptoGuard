#include "playerInfo.hpp"
#include "ui.hpp"
#include <algorithm>
#include <fcntl.h>
#include <iostream>
#include <list>
#include <locale.h>
#include <ncurses.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

using namespace std;

Info::Info() {
    this->score = -1.0;
    memset(this->name, 0x00, MAX_NAME_LEN + 1);
}

Info::Info(string name, int score) {
    memcpy(this->name, name.c_str(), MAX_NAME_LEN);
    this->score = score;
}

void Info::setName(string name) {
    memcpy(this->name, name.c_str(), MAX_NAME_LEN);
}

void Info::setScore(int score) { this->score = score; }

string Info::getName(void) { return string(this->name); }

int Info::getScore(void) { return this->score; }

void fileWrite(int score) {
    setlocale(LC_ALL, "ko_KR.utf8");
    setlocale(LC_CTYPE, "ko_KR.utf8");
    list<Info> InfoList;
    char *tmp;

    char name[MAX_NAME_LEN];
    nodelay(stdscr, FALSE);
    echo();
    mvprintw(UIHEIGHT / 2 + 2, (UIWIDTH - 16) / 2, "Enter the NAME : ");
    refresh();
    fflush(stdin);
    scanw("%s", name);
    nodelay(stdscr, TRUE);
    noecho();

    Info info(name, score);
    InfoList.push_back(info);
    int num = InfoList.size();

    string filepath = "./PlayerInfo.txt";
    int fd = open(filepath.c_str(), O_CREAT | O_APPEND | O_WRONLY, 0644);
    if (fd == -1) {
        perror("open() error!!");
        exit(1);
    }

    list<Info>::iterator iter;
    for (iter = InfoList.begin(); iter != InfoList.end(); ++iter) {
        if (write(fd, &(*iter), sizeof(Info)) == -1) {
            perror("write() error!");
            return;
        }
        /*else
        {
            for (int i = 1; i < InfoList.size(); i++) {
                char cursor = InfoList[i];
                int j = i - 1;
                while ((j >= 0) &&
                       (tmp[j] > cursor)) { // while A[j] is out of order
                    tmp[j + 1] = tmp[j];    // move A[j] right
                    j--;                    // decrement j
                }
                tmp[j + 1] = cursor; // this is the proper place for cur
            }
        } */
        close(fd);
    }
}
void fileRead() {
    setlocale(LC_ALL, "ko_KR.utf8");
    setlocale(LC_CTYPE, "ko_KR.utf8");
    curs_set(0);
    ssize_t rsize = 0;
    int file = 0;
    int k = 0;
    player player[1024], tmp;
    Info info[1024];
    int rank_n = 1; //등수 변수
    int same = 1;   //동점자 처리 변수

    // Info *info = new Info();

    string filepath = "./PlayerInfo.txt";
    file = open(filepath.c_str(), O_RDONLY | O_CREAT, 0644);

    /*if (in.is_open()) {
        in >> s;
        cout << "입력 받은 정보 : " << s << endl;
    } else {
        cout << "파일을 찾을 수 없습니다!" << endl;
    }*/

    do {
        rsize = read(file, (Info *)info, sizeof(Info));

        if (rsize == sizeof(Info)) {
            player[k].setName(info->getName());
            player[k++].setScore(info->getScore());
        } else if (rsize == -1) {
            perror("read() error!");
            exit(-1);
        }
    } while (rsize > 0);
    close(file);

    //랭킹 정렬
    for (int i = k - 1; i > 0; i--) {
        for (int j = 0; j < i; j++) {
            if (player[j].getScore() < player[j + 1].getScore()) {
                SWAP(player[j], player[j + 1], tmp);
            }
            /*else if (player[j].getScore() == player[j+1].getScore())
            {

            }*/
        }
    }

    // 1페이지 이상
    if (k > 23)
        mvprintw(0, (UIWIDTH - 3) / 2, "1/%d", k * 2 / 23);

    //랭킹 출력(23등 까지만 출력)
    // for (int i = 0; i < 23; i++)
    for (int i = 0; i < k; i++) {
        if (i > 22)
            break;
        mvprintw(1 + i, 1, "%d등 NAME : %s SCORE : %d", rank_n,
                 player[i].getName().c_str(), player[i].getScore());
        /*cout << rank_n << "등 "
             << "NAME : " << player[i].getName()
             << " SCORE : " << player[i].getScore() << endl;*/
        if (i < (k - 1)) {
            if (player[i].getScore() == player[i + 1].getScore()) {
                // cout << "cnt : " << cnt << endl;
                ++same;
            } else {
                rank_n += same;
                same = 1;
            }
        }
    }
    mvprintw(UIHEIGHT - 1, (UIWIDTH - strlen("[Press Enter]")) / 2,
             "[Press Enter]");
}

// test용, 4명 및 동점자 처리
/* int main() { // main에서 score 변수값 넣기 score값 임시로 지정
    int score = 100;
    fileWrite(score);
    int score2 = 100;
    fileWrite(score2);
    int score3 = 200;
    fileWrite(score3);
    int score4 = 400;
    fileWrite(score4);

    cout << endl;
    cout << "랭킹 출력" << endl;
    fileRead();

    return 0;

} */
