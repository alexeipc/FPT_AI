#include <iostream>
#include <sstream>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <utility>
#include <chrono>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;
using namespace rapidjson;

#define MAX_SIZE 40000

void getInfor(string json);
void updateData(string json);

/*store the map:
 * 0: land
 * -1: forest
 * -2: trap
 * -3: swamp
 * >0: gold */
vector<vector<int>> gmap;
int playerId, height, width, currentE, maxE, maxT, playerNumbers, startX, startY;



int main(int argc, char *argv[])
{
    char* peerHost = "localhost";
    short peerPort = 4000;
    srand(time(NULL));
	if (argc == 3){
        peerHost = argv[1];
		peerPort = stoi(argv[2]);
	}else if(argc == 2){
		peerPort = (short) atoi(argv[1]);
	}

    // Create socket
    int s0 = socket(AF_INET, SOCK_STREAM, 0);
    if (s0 < 0)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }
 
    // Fill in server IP address
    struct sockaddr_in server;
    int serverAddrLen;
    bzero( &server, sizeof( server ) );
  
    // Resolve server address (convert from symbolic name to IP number)
    struct hostent *host = gethostbyname(peerHost);
    if (host == NULL)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }
 
    server.sin_family = AF_INET;
    server.sin_port = htons(peerPort);
	
    // Write resolved IP address of a server to the address structure
    memmove(&(server.sin_addr.s_addr), host->h_addr_list[0], 4);
 
    // Connect to the remote server
    int res = connect(s0, (struct sockaddr*) &server, sizeof(server));
    if (res < 0)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }
 
    std::cout << "Connected. Reading a server message" << std::endl;
 
    char buffer[MAX_SIZE];
    memset(buffer, 0, MAX_SIZE);
    res = read(s0, buffer, MAX_SIZE);
    if (res < 0)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }
 
    std::cout << "Received: " << buffer;
 
    string raw_json(buffer);
    getInfor(raw_json);

    //0: go left, 1: go right, 2: go up, 3: go down, 4: free, 5: craft
    string command[] = {"0", "1", "2", "3", "4", "5"};
    int count = 0;
	int restcnt = 0;
	int preX = 0;
	int preY = 0;
    int idx = 0;
    for (int i=0; i<maxT; i++)
    {
		if (restcnt > 0) {
			idx = 4;
			restcnt--;
		}
		else {
			if (currentE <= 5)
			{
				idx = 4;//free to get more energy
				restcnt = rand()%3;
			}
			else
			{
				if (gmap[startY][startX] > 0)
				{
					//craff
					idx = 5;
				}
				else
				{
					//go to other 
					bool ok = false;
					while (!ok)
						if (startY > 0 && gmap[startY-1][startX] > 0 && startY - 1 != preY){
							idx = 2;
							ok = true;
						}
						else if (startY < height-1 && gmap[startY+1][startX] > 0 && startY + 1 != preY) {
							idx = 3;
							ok = true;
						}
						else if (startX > 0 && gmap[startY][startX-1] > 0 && startX - 1 != preX) {
							idx = 0;
							ok = true;
						}
						else if (startX < width-1 && gmap[startY][startX+1] > 0 && startX + 1 != preX) {
							idx = 1;
							ok = true;
						}
						else {
							idx = rand()%4;
							if ((idx==0) && (startX>0)) ok = true;
							if ((idx==1) && (startX<width-1)) ok = true;
							if ((idx==2) && (startY>0)) ok = true;
							if ((idx==3) && (startY<height-1)) ok = true;
						}
				}
			}
		}
        preX = startX;
		preY = startY;
        cout << "send command:" << command[idx] << endl;
        //send command to server
        write(s0, command[idx].c_str(), 1);
        
        //recv data from server
        memset(buffer, 0, MAX_SIZE);    
        res = read(s0, buffer, MAX_SIZE);
        cout << "res:" << res << endl;
        string json(buffer);

        cout << "received data: "<< json << endl;
        updateData(json);

    //please insert your process code
	////
	/////
	//////	
    }
    close(s0);
    return 0;
}

void updateData(string json)
{
	//process data
	Document d;
    
	ParseResult result = d.Parse(json.c_str());
    if (!result) {
        cout << "JSON parse error: " << result.Offset() << endl;
        return;
    }

	//update data for player
	Value &array_player = d["players"];
	cout << "playerSize:" << array_player.Size() << endl;
    for (int i = 0; i < array_player.Size(); i++)
	{
		int id = array_player[i]["playerId"].GetInt();
		int x = array_player[i]["posx"].GetInt();
		int y = array_player[i]["posy"].GetInt();
		int gold = array_player[i]["score"].GetInt();
		int energy = array_player[i]["energy"].GetInt64();
		int lastAction = array_player[i]["lastAction"].GetInt64();

		cout << "player " << id << " at (" << y << ", " << x << "), lastAction: "<< lastAction << ", gold: " << gold << ", energy: " << energy << endl;
        if (id == playerId)
        {
            startX = x;
            startY = y;
            currentE = energy;
        }
	}

    //reset golds of map
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j++)
            if (gmap[i][j] > 0)
            {
                gmap[i][j] = 0;
            }
    }
    
    //update the golds
    Value &array_gold = d["golds"];
    for (int i = 0; i < array_gold.Size(); i++)
    {
        int x = array_gold[i]["posx"].GetInt();
        int y = array_gold[i]["posy"].GetInt();
        int amount = array_gold[i]["amount"].GetInt();
        gmap[y][x] = amount;
    }
}

void getInfor(string raw_json){
    StringStream s(raw_json.c_str());
    Document d;
    d.ParseStream(s);

    //start game infor
    playerId = d["playerId"].GetInt();
    width = d["gameinfo"]["width"].GetInt();
    height = d["gameinfo"]["height"].GetInt();
    maxT = d["gameinfo"]["steps"].GetInt();
    playerNumbers = d["gameinfo"]["numberOfPlayers"].GetInt();
    startY = d["posy"].GetInt();
    startX = d["posx"].GetInt();
    currentE = maxE = d["energy"].GetInt();

    //init map with all lands
    for (int i=0; i<height; i++)
        gmap.push_back(vector<int>(width, 0));

    //init map
    Value &array_obstacles = d["gameinfo"]["obstacles"];
    for (int i = 0; i < array_obstacles.Size(); i++)
    {
        int x = array_obstacles[i]["posx"].GetInt();
        int y = array_obstacles[i]["posy"].GetInt();
        int type = array_obstacles[i]["type"].GetInt();
        gmap[y][x] = -type;
    }

    // init data for gold
    Value &array_gold = d["gameinfo"]["golds"];
    for (int i = 0; i < array_gold.Size(); i++)
    {
        int x = array_gold[i]["posx"].GetInt();
        int y = array_gold[i]["posy"].GetInt();
        int gold = array_gold[i]["amount"].GetInt();
        gmap[y][x] = gold;
    }
    cout << "width, height:" << width << " " << height << endl;
    cout << "maxE, maxT: " << maxE << " " << maxT << endl;
    cout << "numOfPlayer, startPos:" << playerNumbers << "(" << startX << "," << startY << ")" << endl;
    // dump the map
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j++)
            cout << gmap[i][j] << " ";
        cout << endl;
    }
}
