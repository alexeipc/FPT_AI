#include <iostream>
#include <sstream>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;

/*
   store the map:
 * 0: land
 * -1: forest
 * -2: trap
 * -3: swamp
 * >0: gold 
 */

vector<vector<int>> gmap;
int playerId, height, width, currentE, maxE, maxT, playerNumbers, startX, startY;
int alive[5],energy[5],lastAction[5];
pair<int,int> pos[5];
queue<int> Trace;

int find_gold();
void getInfor() {
	// Input
	cin >> playerId >> width >> height >> maxT;
	cin >> playerNumbers >> startX >> startY >> currentE;
	
	// Init
	maxE = currentE;
	for (int i=0; i<height; ++i) 
		gmap.push_back(vector<int>(width,0));
	
	// obstacles
	int obstacles; cin >> obstacles;
	for (int i=0; i<obstacles; ++i) {
		int x,y,type;
		cin >> x >> y >> type;
		gmap[y][x] = -type;
	}
	
	// gold
	int gold; cin >> gold;
	for (int i=0; i<obstacles; ++i) {
		int x,y,val;
		cin >> x >> y >> val;
		gmap[y][x] = val;
	}
	
	// Print current map
	for (int i=0; i<height; ++i) {
		for (int j=0; j<width; ++j) cout << gmap[i][j] << " ";
		cout << endl;
	}
	
	for (int i=0; i<=n; ++i) alive[i] = 1;
}

void control(int id, int action) {
	int x = pos[id].first;
	int y = pos[id].second;
	
	int p[4] = {-1,1,0,0};
	int h[4] = {0,0,-1,1};
	
	if (0 <= action <= 3) {
		// Move	
		
		y += p[action];
		x += h[action];
		
		int minus = 0;
		if (gmap[x][y] > 0) minus = 4;
		else if (gmap[x][y] == 0) minus = 1;
		else if (gmap[x][y] == -2) minus = 2;
		else if (gmap[x][y] == -1 || gmap[x][y] == -3) minus = 3;
		enery[id] -= minus;
	}
	else if (action == 5) {
		// craff
	
		energy[id] -= 5;
		gmap[x][y] -= 50;
	}
	else if (action == 4) {
		if (lastaction[id] == 4) {
			rest[id]--;
			if (rest[id] != 0) energy[id] += maxE/rest[id];
		}
		else {
			energy[id] +=maxE/4;
			rest[id] = 4;
		}
		energy[id] = min(maxE,energy[id]); 
	}
	
	lastaction[id] = action;
	
	cout << "PLayer " << id << " at (" << x << "," << y << ") ";
	cout << "energy " << energy[id] << endl;
}

void updateData(int x) {
	cout << playerNumbers << endl;
	for (int i=0; i<=n; ++i) {
		if (alive[i] && i != playerId) {
			int action; cin >> action;
			control(i,action);
		}
		else if (i == playerId) {
			control(i,x);
		}
	}
}

int main(int argc, char *argv[]) {
	getInfor();
	string command[] = {"0","1","2","3","4","5"};
    int count = 0;
    int idx = 0;
    
    for (int i=0; i<maxT; ++i) {
    	if (currentE <= 5) idx = 4;
    	else {
    		if (gmap[startY][startX] > 0) {
    			idx = 5; // craff
    			// Find new gold
    			if (gmap[startY][startX] - 50 <= 0) {
    				Trace = {};
    				find_gold(startY,startX)
    			}
    		}
    		else {
    			idx = find_gold(startX,startY);
    		}
    		
    		cout << "send command:" << command[idx] << endl;
    		
    		updateData(idx);
    	}
    	
    }
}

// FIND GOLD
int arr[height][width];
struct gold{ int x,y,val; };
bool cmp(gold a, gold b) {
	return (a.val > b.val);
}

bool check(int x, int y, int xx, int yy) {
	if (x >= height || x<=0) return 0;
	if (y >= width || y<=0) return 0;
	if (arr[x][y]) return 0;
	if (gmap[xx][yy] < 0 && gmap[x][y] < 0) return 0;
	
	if (gmap[xx][yy] < 0) {
		int minus = 0;
		if (gmap[xx][yy] == -1) minus = 3;
		else if (gmap[xx][yy] == -2) minus = 2;
		else if (gmap[xx][yy] == -3) minus = 1;
		
		int p1 = e/4;
		int p2 = e/4 + e/3;
		int p3 = e/4 + e/3 + e/2;
		
		if (p1 > min) return 1;
		else if (p2 > min) return 1;
		else if (p3 > min) return 1;
	}
	else return 1;
}

void find_gold(int currY, int currX) {
	// find gold
	vector<gold> coordinates;
	for (int i=0; i<height; ++i)
		for (int j=0; j<width; ++j) {
			if (gmap[i][j] > 0) {
				gold pt;
				pt.x = i; pt.y = j; pt.val = gmap[i][j];
				
				coordinates.push_back(pt);
			}
		}
		
	sort(coordinates.begin(), coordinates.end(), cmp);
	
	// BFS
	
	queue<pair<int,int>> qu = {{startX,startY}};
	arr[startX][startY] = 1;
	
	while (!qu.empty()) {
		int x = qu.top().first;
		int y = qu.top().second;
		qu.pop();
		
		if (check(x+1,y,x,y)) {
			qu.push({x+1,y});
			arr[x+1][y] = arr[x][y] + 1;
		}
		else if (check(x-1,y,x,y)) {
			qu.push({x-1,y});
			arr[x-1][y] = arr[x][y] + 1;
		}
		else if (check(x,y+1,x,y)) {
			qu.push({x,y+1});
			arr[x][y+1] = arr[x][y] + 1;
		}
		else if (check(x,y-1,x,y)) {
			qu.push({x,y-1});
			arr[x][y-1] = arr[x][y] + 1;
		}
	}
	
	// Find best sollution
	for (int g=0; g<coordinates.size(); ++g) {
		bool check = 0;
		// Check
		
		
		
		// Can we go
		if (check) return;
	}
}
