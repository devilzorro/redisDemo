#include <iostream>
#include <stdlib.h>
#include <thread>
#include "json/json.h"
#include "redisClient/redisClient.h"

#include <sstream>
#include <chrono>
#include <ctype.h>
#include <vector>


#define random(x) (rand()%x)
#ifdef WIN32
#include <Windows.h>
#include "win_stdafx/stdafx.h"
#pragma comment(lib,"ws2_32.lib")
#else

#endif

using namespace std;

CRedisClient redisClient;

string str2010;
string str2011;
vector<string> vcRedisList;

string machineID = "lastMessage_iSC_00E04C68759B";

string set2010() {
    string retData = "";
    Json::Value dataRoot;
    Json::Value root;
    stringstream ss;
    string strVal = "";
    ss<<random(100);
    ss>>strVal;
    cout<<"tool test val:"<<strVal<<endl;
    root["cnc_rdspmeter[0]"] = strVal;
    retData = root.toStyledString();

    return retData;
}

string set2011(string toolNo,string programName,
        string jobCountByStatus,string status,
        string startTime,string endTime,string resetStatus) {
    string retData = "";
    Json::Value root;
    root["ext_toolno"] = toolNo;
    root["cnc_exeprgname"] = programName;
    root["jobCounteByStatus"] = jobCountByStatus;
    root["cnc_statinfo[3]"] = status;
    root["programStartTime"] = startTime;
    root["programEndTime"] = endTime;
    root["pmc_rdpmcrng[1,1,1,1]"] = resetStatus;
    retData = root.toStyledString();
    return retData;
}

void write2010() {
    while (1) {
        cout<<"2010 thread:"<<this_thread::get_id()<<endl;
        Json::Value root;
        this_thread::sleep_for(chrono::milliseconds(20));
        cout<<set2010()<<endl;

        string store2010 = "";
//        redisClient.HGet(machineID,"2010",store2010);
//        redisClient.HSet(machineID,"2010",set2010());
        root["order"] = "2010";
        root["content"] = set2010();
        vcRedisList.push_back(root.toStyledString());
    }
}

void write2011() {
    int count = 0;
    int startTime = 1;
    int endTime = 1;
    string strStartTime = "";
    string strEndTime = "";
    stringstream ssStart;
    stringstream ssEnd;
    ssStart<<startTime;
    ssStart>>strStartTime;
    ssEnd<<endTime;
    ssEnd>>strEndTime;
    string tmp2011 = set2011("12","test.iso","false","1",strStartTime,strEndTime,"0");
    redisClient.HSet(machineID,"2011",tmp2011);
    while (1) {
        cout<<"2011 thread:"<<this_thread::get_id()<<endl;
        this_thread::sleep_for(chrono::seconds(20));
        if (count%2 == 0) {
            startTime++;
            stringstream tmpss;
            tmpss<<startTime;
            tmpss>>strStartTime;

        } else {
//            if (count != 1) {
//
//            }
            endTime++;
            stringstream tmpss;
            tmpss<<endTime;
            tmpss>>strEndTime;
        }
        string tmpData = set2011("12","test.iso","false","1",strStartTime,strEndTime,"0");
        cout<<tmpData<<endl;

        string store2011 = "";
        Json::Value root;
        root["order"] = "2011";
        root["content"] = tmpData;
        vcRedisList.push_back(root.toStyledString());
//        redisClient.HGet(machineID,"2011",store2011);
//        redisClient.HSet(machineID,"2011",tmpData);
        count++;
    }
}

void writeRedis() {
    while (1) {
        this_thread::sleep_for(chrono::milliseconds(10));
        if (!vcRedisList.empty()) {
            Json::Reader reader;
            Json::Value root;
            string strJson = "";
            vector<string>::iterator it = vcRedisList.begin();
            strJson = (*it);
            vcRedisList.erase(it);
            if (reader.parse(strJson,root)) {
                string order = root["order"].asString();
                string content = root["content"].asString();
                redisClient.HSet(machineID,order,content);
            }
        }
    }

}


int main() {
    vcRedisList.clear();
    redisClient.Connect("127.0.0.1",6379);

    if (redisClient.CheckStatus()) {

    } else {
        return 1;
    }

//    int icount = 0;

    thread th2010(write2010);
    thread th2011(write2011);
    thread thWriteRedis(writeRedis);
    th2010.detach();
    th2011.detach();
    thWriteRedis.detach();


    cout<<"input q to exit"<<endl;
    while (tolower(std::cin.get()) != 'q') {

    }

//    while (1) {
//        cout<<"main thread:"<<this_thread::get_id()<<endl;
//        chrono::milliseconds dura(10000);
//        this_thread::sleep_for(dura);
//        if (redisClient.CheckStatus()) {
//            icount++;
//            redisClient.HSet(machineID,"2010",set2010());
//            if (icount <= 500) {
//                string tmp2011 = set2011("12","test.iso","false","1","1333:55","1666:8");
//                redisClient.HSet(machineID,"2011",tmp2011);
//            } else if (icount >500 && icount <= 1000){
//                string tmp2011 = set2011("12","test.iso","false","1","1333:66","1666:8");
//                redisClient.HSet(machineID,"2011",tmp2011);
//            } else if (icount > 1000 && icount <= 1500){
//                string tmp2011 = set2011("12","test.iso","false","1","1333:66","1666:88");
//                redisClient.HSet(machineID,"2011",tmp2011);
//            } else if (icount > 1500 && icount <= 2000){
//                string tmp2011 = set2011("12","test.iso","false","1","1333:77","1666:88");
//                redisClient.HSet(machineID,"2011",tmp2011);
//            } else if (icount > 2000 && icount <= 2500){
//                string tmp2011 = set2011("12","test.iso","false","1","1333:77","1666:99");
//                redisClient.HSet(machineID,"2011",tmp2011);
//            } else {
//                icount = 0;
//            }
//
//        }
//    }

    return 0;
}