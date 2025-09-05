#include <bits/stdc++.h>
using namespace std;

struct State{
    int r, c, ori; // ori = 0 -> horizontal (anchor at left cell), 1 -> vertical (anchor at top cell)
    int d;         // distance (steps)
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int M, N;
    if(!(cin >> M >> N)) return 0;

    vector<vector<char>> g(M, vector<char>(N));
    vector<pair<int,int>> startCells, goalCells;

    for(int i=0;i<M;i++){
        for(int j=0;j<N;j++){
            string t; cin >> t; 
            char ch = t[0];
            g[i][j] = ch;
            if(ch=='s') startCells.push_back({i,j});
            if(ch=='S') goalCells.push_back({i,j});
        }
    }

    // Treat 's' and 'S' as free spaces for movement
    for(auto [r,c]: startCells) g[r][c] = '0';
    for(auto [r,c]: goalCells) g[r][c] = '0';

    auto freeCell = [&](int r,int c)->bool{
        return r>=0 && r<M && c>=0 && c<N && g[r][c] != 'H';
    };

    // Build start state (anchor + orientation)
    if(startCells.size()!=2 || goalCells.size()!=2){
        cout << "Impossible\n";
        return 0;
    }

    // start orientation and anchor
    sort(startCells.begin(), startCells.end());
    int sr1 = startCells[0].first, sc1 = startCells[0].second;
    int sr2 = startCells[1].first, sc2 = startCells[1].second;

    int sOri, sR, sC; // anchor
    if(sr1 == sr2){ // horizontal
        sOri = 0;
        sR = sr1;
        sC = min(sc1, sc2);
    }else{          // vertical
        sOri = 1;
        sR = min(sr1, sr2);
        sC = sc1; // after sort, smaller row's column
        if(sr1 > sr2) sC = sc2;
    }

    // goal pair (unordered)
    pair<int,int> g1 = goalCells[0], g2 = goalCells[1];

    auto isGoal = [&](int r,int c,int ori)->bool{
        pair<int,int> a={r,c};
        pair<int,int> b = (ori==0) ? make_pair(r, c+1) : make_pair(r+1, c);
        return ( (a==g1 && b==g2) || (a==g2 && b==g1) );
    };

    // visited[r][c][ori] for valid anchors only
    vector<vector<array<bool,2>>> vis(M, vector<array<bool,2>>(N, {false,false}));
    queue<State> q;

    auto push_if_ok = [&](int r,int c,int ori,int d){
        // check anchor fits and both cells are free
        if(ori==0){
            if(!(r>=0 && r<M && c>=0 && c+1<N)) return;
            if(!(freeCell(r,c) && freeCell(r,c+1))) return;
        }else{
            if(!(r>=0 && r+1<M && c>=0 && c<N)) return;
            if(!(freeCell(r,c) && freeCell(r+1,c))) return;
        }
        if(!vis[r][c][ori]){
            vis[r][c][ori]=true;
            q.push({r,c,ori,d});
        }
    };

    // seed
    push_if_ok(sR, sC, sOri, 0);

    // BFS
    while(!q.empty()){
        auto cur = q.front(); q.pop();
        int r=cur.r, c=cur.c, o=cur.ori, d=cur.d;
        if(isGoal(r,c,o)){
            cout << d << "\n";
            return 0;
        }

        if(o==0){ // horizontal: cells (r,c) & (r,c+1)
            // translate: up/down/left/right
            if(r-1>=0 && freeCell(r-1,c) && freeCell(r-1,c+1)) push_if_ok(r-1,c,0,d+1);
            if(r+1<M  && freeCell(r+1,c) && freeCell(r+1,c+1)) push_if_ok(r+1,c,0,d+1);
            if(c-1>=0 && freeCell(r,c-1))                       push_if_ok(r,c-1,0,d+1);
            if(c+2<N  && freeCell(r,c+2))                       push_if_ok(r,c+1,0,d+1);

            // rotate within 2x2 blocks (up and down), both pivots allowed
            if(r-1>=0 && freeCell(r-1,c) && freeCell(r-1,c+1)){
                // rotate to vertical at left column or right column
                push_if_ok(r-1,c,1,d+1);     // using column c
                push_if_ok(r-1,c+1,1,d+1);   // using column c+1
            }
            if(r+1<M && freeCell(r+1,c) && freeCell(r+1,c+1)){
                push_if_ok(r,  c, 1,d+1);    // column c
                push_if_ok(r,  c+1,1,d+1);   // column c+1
            }
        }else{ // vertical: cells (r,c) & (r+1,c)
            // translate
            if(r-1>=0 && freeCell(r-1,c))                         push_if_ok(r-1,c,1,d+1);
            if(r+2<M  && freeCell(r+2,c))                         push_if_ok(r+1,c,1,d+1);
            if(c-1>=0 && freeCell(r,c-1) && freeCell(r+1,c-1))    push_if_ok(r,c-1,1,d+1);
            if(c+1<N  && freeCell(r,c+1) && freeCell(r+1,c+1))    push_if_ok(r,c+1,1,d+1);

            // rotate within 2x2 blocks (left and right), both pivots allowed
            if(c-1>=0 && freeCell(r,c-1) && freeCell(r+1,c-1)){
                push_if_ok(r,  c-1,0,d+1);   // row r
                push_if_ok(r+1,c-1,0,d+1);   // row r+1
            }
            if(c+1<N && freeCell(r,c+1) && freeCell(r+1,c+1)){
                push_if_ok(r,  c,  0,d+1);   // row r
                push_if_ok(r+1,c,0,d+1);     // row r+1
            }
        }
    }

    cout << "Impossible\n";
    return 0;
}
