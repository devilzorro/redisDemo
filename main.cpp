#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include "json/json.h"
#include "redisClient/redisClient.h"
#include <iomanip>

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

string strStartTime = "1";
string strEndTime = "1";
string programName = "test.iso";
string toolNo = "12";

vector<string> vcRedisList;

string machineID = "iFC_00e0e41f0113";

string setVal2010= "";
string setRpm = "";
string set2010Order = "2010";

string getSysTime(){
    //format: [s : ms]
    struct timeval tv;
    gettimeofday(&tv,NULL);
    char s[128] = "";
    sprintf(s,"%ld:%ld",tv.tv_sec,tv.tv_usec/1000);
    string str = s;
    return str;
}

string set2010() {
    string retData = "";
    Json::Value dataRoot;
    Json::Value root;
//    stringstream ss;
    string strVal = "";
    string tmpSetRpm = "";
//    ss<<random(100);
//    ss>>strVal;
    if (setVal2010 != "") {
        strVal = setVal2010;
    }

    if (setRpm != "") {
        tmpSetRpm = setRpm;
    }
    cout<<"tool test val:"<<strVal<<endl;
    root["cnc_rdspmeter[0]"] = strVal;
    root["cnc_acts"] = tmpSetRpm;
    root["isesol_timeStamp"] = getSysTime();
    string tmpStr = root.toStyledString();
    dataRoot["type"] = "2210";
    dataRoot["val"] = tmpStr;
    dataRoot["machineType"] = "fanuc";
    retData = dataRoot.toStyledString();
    return retData;
}

string set2011(string toolNo,string programName,
        string jobCountByStatus,string status,
        string startTime,string endTime,string resetStatus) {
    string retData = "";
    Json::Value rootBase;
    Json::Value root;
    root["ext_toolno"] = toolNo;
    root["cnc_rdprgnum[1]"] = programName;
    root["jobCounteByStatus"] = jobCountByStatus;
    root["cnc_statinfo[3]"] = status;
    root["programStartTime"] = startTime;
    root["programEndTime"] = endTime;
    root["pmc_rdpmcrng[1,1,1,1]"] = "640";
    string tmpStr = root.toStyledString();
    rootBase["type"] = "2211";
    rootBase["val"] = tmpStr;
    rootBase["machineType"] = "fanuc";
    retData = rootBase.toStyledString();
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
        root["order"] = set2010Order;
        root["content"] = set2010();
        vcRedisList.push_back(root.toStyledString());
    }
}

void write2011() {
    string tmp2011 = set2011(toolNo,programName,"false","1",strStartTime,strEndTime,"0");
    redisClient.HSet(machineID,"2211",tmp2011);
    while (1) {
        cout<<"2011 thread:"<<this_thread::get_id()<<endl;
        this_thread::sleep_for(chrono::milliseconds(500));

        string tmpData = set2011(toolNo,programName,"false","1",strStartTime,strEndTime,"0");
        cout<<tmpData<<endl;

        string store2011 = "";
        Json::Value root;
        root["order"] = "2211";
        root["content"] = tmpData;
        vcRedisList.push_back(root.toStyledString());
//        redisClient.HGet(machineID,"2011",store2011);
//        redisClient.HSet(machineID,"2011",tmpData);
    }
}

void changeTimePoint() {
    int count = 0;
    int startTime = 1;
    int endTime = 1;
    stringstream ssStart;
    stringstream ssEnd;
    while (1) {
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

            //change programName
            this_thread::sleep_for(chrono::seconds(2));
            string strCount;
            stringstream sName;
            sName<<count;
            sName>>strCount;
            programName = "test" + strCount + ".iso";
        }
        count++;
    }
}

void changetoolNo() {
    int itoolNo = 12;
    while (1) {
        this_thread::sleep_for(chrono::milliseconds(1000));
        if ((itoolNo >= 12)&&(itoolNo <= 15)) {
            itoolNo++;
            stringstream ssToolNo;
            ssToolNo<<itoolNo;
            ssToolNo>>toolNo;
        } else {
            itoolNo = 12;
        }
    }
}

//void changeProgramName() {
//
//}

void writeRedis() {
    while (1) {
        this_thread::sleep_for(chrono::milliseconds(50));
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
                string tmpMachineId = "lastMessage_" + machineID;
                redisClient.HSet(tmpMachineId,order,content);
            }
        }
    }

}

void timeJson() {
    while (1) {
        this_thread::sleep_for(chrono::seconds(5));
        string strTime = "1554107835.486";
        double dbTime;
        stringstream ssTime;
        ssTime<<strTime;
        ssTime>>setprecision(4)>>dbTime;
        Json::Value root;
        Json::Value arrayRoot;
        Json::Value obj;
        obj["toolnum"] = 1;
        obj["load"] = 7.8;
        obj["time"] = dbTime;
        arrayRoot.append(obj);
        root["feeddata"] = arrayRoot;
        cout<<root.toStyledString()<<endl;
    }
}


int main(int argc,char *argv[]) {
    string strOrder;
    if (argc > 1) {
        strOrder = argv[1];
    }

    if (strOrder == "2210") {
        set2010Order = "2210";
        if (argc > 2) {
            setVal2010 = argv[2];
        }
        if (argc > 3) {
            setRpm = argv[3];
        }
        if (argc > 4) {
            machineID = argv[4];
        }
    } else if (strOrder == "2211") {
        if (argc > 2) {
            toolNo = argv[2];
        }
        if (argc > 3) {
            //start time
            strStartTime = argv[3];
        }
        if (argc > 4) {
            //stop time
            strEndTime = argv[4];
        }
        if (argc > 5) {
            programName = argv[5];
        }
        if (argc > 6) {
            machineID = argv[6];
        }

    } else {

    }

//    if (argc > 1) {
//        setVal2010 = argv[1];
//    }
//    if (argc > 2) {
//        setRpm = argv[2];
//    }
//
//    if (argc > 3) {
//        strOrder = argv[3];
//        if (strOrder == "2030") {
//            set2010Order = strOrder;
//        } else if (strOrder == "2031"){
//
//        }
//        set2010Order
//    }
//
//    if (argc > 4) {
//        machineID = argv[4];
//    }

    vcRedisList.clear();
    redisClient.Connect("127.0.0.1",6379);

    if (redisClient.CheckStatus()) {

    } else {
        return 1;
    }

//    int icount = 0;

//    thread thChangeTimePoint(changeTimePoint);
//    thread thChangeProName(changeProgramName);
//    thread thChangeToolNo(changetoolNo);
    thread thWriteRedis(writeRedis);
    if (strOrder == "2210") {
        thread th2010(write2010);
        th2010.detach();
    } else if (strOrder == "2211") {
        thread th2011(write2011);
        th2011.detach();
    } else {

    }

//    th2011.detach();
//    thChangeProName.detach();
//    thChangeTimePoint.detach();
//    thChangeToolNo.detach();

    thWriteRedis.detach();

//    thread thTimeJson(timeJson);
//    thTimeJson.detach();

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