#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstdlib>  // 用于随机数
#include <ctime>
using namespace std;

bool bfs(int size,vector<vector<char>> &maze,vector<pair<int,int>> &bfs_result) ;
//bfs函数用来判断函数是否有解同时返回参考路线用来和训练结果比较 

void train(int size,int episodes,vector<vector<char>> &maze,vector<vector<vector<double>>> &q_table,vector<vector<int>> &reward);
//Q-learning训练函数 

int main()
{
	cout << "输入迷宫尺寸（整数） ： ";
	int size;
	cin >> size;
	cout << "输入训练轮数（整数） ： ";
	int episodes;
	cin >> episodes;
	
	vector<vector<char>> maze(size,vector<char>(size,'.')); //迷宫 
	vector<vector<vector<double>>> q_table(size,vector<vector<double>>(size,vector<double>(4,0))) ;  //Q表
	vector<vector<int>> reward(size,vector<int>(size,-1)); //奖励 
	
	vector<pair<int,int>> bfs_result;//bfs参考路线 
	
	maze[0][0] = 'S';
	maze[size-1][size-1] = 'E';
	reward[size-1][size-1] = 100;
	//初始化 
	 
	cout << "输入障碍的横纵坐标（除起点和终点），空格隔开。输入任意其他字符退出：" << endl;
	int x,y;
	while (cin >> x >> y && x >= 0 && x < size && y >= 0 && y < size && !(x==0&&y==0) && !(x==size-1&&y==size-1)) 
	{
	    maze[x][y] = '#';
	    reward[x][y] = -10;
	    cout << "继续输入障碍的横纵坐标（除起点和终点），输入其他字符退出：" << endl;
	}
	
	cout << "显示迷宫" << endl; 
	for (int i=0;i<size;i++)
	{
		for(int j=0;j<size;j++)
		{
			if (maze[i][j] == 'S') {
                cout << "\033[31m" << 'S' << "\033[0m"; // 红色
            } else if (maze[i][j] == 'E') {
                cout << "\033[31m" << 'E' << "\033[0m"; // 红色
            } else if (maze[i][j] == 'x') {
                cout << "\033[32m" << 'x' << "\033[0m"; // 绿色
            } else if (maze[i][j] == '#') {
                cout << "\033[33m" << '#' << "\033[0m"; // 黄色
            } else {
                cout << maze[i][j]; // 默认颜色
            }
            cout << ' ';
		}
		cout << endl;
	}
	
	if (bfs(size,maze,bfs_result)) 
	{
		train(size,episodes,maze,q_table,reward);
		
		cout << "显示迷宫" << endl; 
		for (int i=0;i<size;i++)
		{
			for(int j=0;j<size;j++)
			{
				if (maze[i][j] == 'S') {
	                cout << "\033[31m" << 'S' << "\033[0m"; // 红色
	            } else if (maze[i][j] == 'E') {
	                cout << "\033[31m" << 'E' << "\033[0m"; // 红色
	            } else if (maze[i][j] == 'x') {
	                cout << "\033[32m" << 'x' << "\033[0m"; // 绿色
	            } else if (maze[i][j] == '#') {
	                cout << "\033[33m" << '#' << "\033[0m"; // 黄色
	            } else {
	                cout << maze[i][j]; // 默认颜色
	            }
	            cout << ' ';
			}
			cout << endl;
		}
		
		cout << "bfs参考路径" <<endl;
		vector<vector<char>> copy1 = maze; 
		for (auto point:bfs_result)
		{
			copy1[point.first][point.second] = 'x';
		}
		for (int i=0;i<size;i++)
		{
			for(int j=0;j<size;j++)
			{
				if (copy1[i][j] == 'S') {
	                cout << "\033[31m" << 'S' << "\033[0m"; // 红色
	            } else if (copy1[i][j] == 'E') {
	                cout << "\033[31m" << 'E' << "\033[0m"; // 红色
	            } else if (copy1[i][j] == 'x') {
	                cout << "\033[32m" << 'x' << "\033[0m"; // 绿色
	            } else if (copy1[i][j] == '#') {
	                cout << "\033[33m" << '#' << "\033[0m"; // 黄色
	            } else {
	                cout << copy1[i][j]; // 默认颜色
	            }
	            cout << ' ';
			}
			cout << endl;
		}
		
		cout << "Q-learning学习路径" <<endl;
		vector<vector<char>> copy2 = maze; 
		int x=0,y=0;
		int count = 0;
		while(!(x==size-1&&y==size-1)&&count<size*size)
		{
			int dx[] = {1, -1, 0, 0};
	    	int dy[] = {0, 0, 1, -1};
	    	int action = max_element(q_table[x][y].begin(), q_table[x][y].end()) - q_table[x][y].begin();
	    	int nx = x+dx[action];
	    	int ny = y+dy[action];
	    	copy2[nx][ny] = 'x';
	    	x = nx;
	    	y = ny;
	    	count++;
		} 
		if (count=size*size) cout << "训练次数过少" << endl; 
		else
		{
			for (int i=0;i<size;i++)
			{
				for(int j=0;j<size;j++)
				{
					if (copy2[i][j] == 'S') {
		                cout << "\033[31m" << 'S' << "\033[0m"; // 红色
		            } else if (copy2[i][j] == 'E') {
		                cout << "\033[31m" << 'E' << "\033[0m"; // 红色
		            } else if (copy2[i][j] == 'x') {
		                cout << "\033[32m" << 'x' << "\033[0m"; // 绿色
		            } else if (copy2[i][j] == '#') {
		                cout << "\033[33m" << '#' << "\033[0m"; // 黄色
		            } else {
		                cout << copy2[i][j]; // 默认颜色
		            }
		            cout << ' ';
				}
				cout << endl;
			}
		}
	}
	
	else cout << "迷宫无解" << endl; 
	 
	return 0;
}

bool bfs(int size,vector<vector<char>> &maze,vector<pair<int,int>> &bfs_result)
{
	bool visited[size][size] = {0};
	int dx[] = {1,-1,0,0};
	int dy[] = {0,0,1,-1};
	queue<pair<int,int>> q;
	vector<vector<pair<int, int>>> ref(size, vector<pair<int, int>>(size, {-1, -1})); //回溯 
    q.push({0,0});
    visited[0][0] = 1;
    
    while (!q.empty()) {
        int s = q.size(); 
        for (int i = 0; i < s; i++) {
            pair<int, int> current = q.front();
            q.pop();

            for (int j = 0; j < 4; j++) {
                int x1 = current.first + dx[j];
                int y1 = current.second + dy[j];

                if (x1 >= 0 && x1 < size && y1 >= 0 && y1 < size && !visited[x1][y1] &&maze[x1][y1]!='#') {
                    visited[x1][y1] = 1;
                    ref[x1][y1] = {current.first, current.second};
                    q.push({x1, y1});
                    
                    if (x1 == size-1 && y1 == size-1) {
                        {
                        	int x=x1,y=y1;
                        	while(x!=0||y!=0) 
                        	{
                        		//cout << x << y << endl;
                        		bfs_result.push_back({x,y});
                        		int x0 = x,y0=y;
                        		x = ref[x0][y0].first;
                        		y = ref[x0][y0].second;
							}
							reverse(bfs_result.begin(),bfs_result.end());
							return true;
						}
                    }
                }
            }
        }
    }
    return false;
}

void train(int size, int episodes, vector<vector<char>> &maze, vector<vector<vector<double>>> &q_table, vector<vector<int>> &reward) 
{   
    int dx[] = {1, -1, 0, 0};
    int dy[] = {0, 0, 1, -1};
    double alpha = 0.1, gamma = 0.95, epsilon = 1.0, epsilon_decay = 0.995, epsilon_min = 0.1;

    srand(static_cast<unsigned>(time(0))); // 初始化随机数种子

    for (int i = 0; i < episodes; i++) 
	{
        int x = 0, y = 0; 
        bool done = false;
        int total_reward = 0;

        while (!done) 
		{
            int action;
            double random_value = (double)rand() / RAND_MAX;
            if (random_value < epsilon) 
			{
                // 随机动作
                action = rand() % 4;
            } 
			else 
			{
                // 根据 Q 表选择最优动作
                action = max_element(q_table[x][y].begin(), q_table[x][y].end()) - q_table[x][y].begin();
            }

            int nx = x + dx[action];
            int ny = y + dy[action];

            if (nx < 0 || nx >= size || ny < 0 || ny >= size) 
			{
                nx = x;
                ny = y;
            }

            // 计算奖励和更新 Q 表
            int r = reward[nx][ny];
            double max_q_next = *max_element(q_table[nx][ny].begin(), q_table[nx][ny].end());
            q_table[x][y][action] += alpha * (r + gamma * max_q_next - q_table[x][y][action]);

            x = nx;
            y = ny;
            total_reward += r;

            if (maze[x][y] == 'E') 
			{
                done = true;
            }
        }

        epsilon = max(epsilon_min, epsilon * epsilon_decay);

        cout << "Episode " << i + 1 <<'/' << episodes << "  total reward: " << total_reward <<"  epsilon: "<< epsilon <<endl;
    }
    
    return;
}

 
