#ifndef __PLAYERINFO_H__
#define __PLAYERINFO_H__
#include <algorithm>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <list>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

using namespace std;

#define SWAP(x, y, t) ((t) = (x), (x) = (y), (y) = (t))
#define MAX_NAME_LEN 64

// player class
class player {
  private:
    string name;
    int score;

  public:
    string getName() { return name; }
    int getScore() { return score; }
    void setName(string name) { this->name = name; }
    void setScore(double score) { this->score = score; }
};

// infomation class
class Info {
  private:
    char name[MAX_NAME_LEN + 1];
    int score;

  public:
    Info();
    Info(string name, int score);

    void setName(string name);
    void setScore(int score);

    string getName(void);
    int getScore(void);
};

//파일에 이름과 점수 기록용 함수
void fileWrite(int score);
//파일 읽어오는 함수
void fileRead();

#endif
