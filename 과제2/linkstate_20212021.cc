#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <utility>
#include <cstring>

using namespace std;

const int INF = 1e9; 

class l_state {
private:
    int node_count;
    vector<pair<int, int>> temp[101];
    int D[101][101];
    int pre_state[101][101];

    void init() {
        int i = 0;
        while (i < node_count) {
            int j = 0;
            while (j < node_count) {
                D[i][j] = INF;
                D[j][i] = INF;
                pre_state[i][j] = -1;
                if (i == j) {
                    D[i][j] = 0;
                    pre_state[i][j] = i;
                }
                ++j;
            }
            ++i;
        }
    }

    void p_message(ofstream& fp1, ifstream& fp2) {
        int start, end;
        int next_one;
        char message[1000];
        char* one_line;
        while (fp2 >> start >> end) {
            fp2.getline(message, 1000);
            one_line = message;
            if (D[start][end] != INF) {
                fp1 << "from " << start << " to " << end << " cost " << D[start][end] << " hops ";
                next_one = start; 
                vector<int> temp_route;
                do {
                    temp_route.push_back(next_one);
                    next_one = pre_state[next_one][end];
                } while (next_one != end);

                for (const auto& route : temp_route) {
                    fp1 << route << " ";
                }

                fp1 << "message" << one_line << endl;
            }
            else {
                fp1 << "from " << start << " to " << end << " cost infinite hops unreachable message" << one_line << endl;
            }
        }
        fp1 << endl;
    }

    void dijk(ofstream& fp) {
        init();
        for (int i = 0; i < node_count; i++) {
            int F = 0;
            priority_queue<pair<int, int>> arr;
            arr.emplace(0, -i);
            while (!arr.empty()) {
                auto top = arr.top();
                int weight = -top.first;
                int now = -top.second;

                arr.pop();
                if (weight > D[i][now]) {
                    continue;
                }
                for (const auto& edgeInfo : temp[now]) {
                    int nextnode = edgeInfo.first;
                    int cost = edgeInfo.second;
                    if (D[i][nextnode] > weight + cost) {
                        D[i][nextnode] = weight + cost;
                        pre_state[i][nextnode] = (F == 0) ? nextnode : pre_state[i][now];
                        arr.emplace(-D[i][nextnode], -nextnode);
                    }
                }
                F += 1;
            }
            int j = 0;
            while (j < node_count) {
                if (D[i][j] != INF) {
                    fp << j << " " << pre_state[i][j] << " " << D[i][j] << endl;
                }
                ++j;
            }

            fp << endl;
        }
    }

public:
    l_state(int n) : node_count(n) {}

    void process(const char* topo_File, const char* mess_File, const char* chang_File) {
        ifstream fp1(topo_File), fp2(mess_File), fp3(chang_File);
        ofstream result("output_ls.txt");
        int s_node, e_node, dist;
        char change[20];

        if (!fp1 || !fp2 || !fp3) {
            cerr << "Error: open input file." << endl;
            return;
        }

        if (fp1 >> node_count) {
            while (fp1 >> s_node >> e_node >> dist) {
                temp[s_node].push_back(make_pair(e_node, dist));
                temp[e_node].push_back(make_pair(s_node, dist));
            }
        }

        dijk(result);
        p_message(result, fp2);
        fp2.close();

        while (fp3.getline(change, sizeof(change))) {
            int F1 = 0;
            int F2 = 0;
            ifstream fp2(mess_File);
            sscanf(change, "%d %d %d", &s_node, &e_node, &dist);
            int r_dist = (dist == -999) ? INF : dist;

            for (auto& temp_Info : temp[s_node]) {
                if (temp_Info.first == e_node) {
                    temp_Info.second = r_dist;
                    F1 = 1;
                    break;
                }
            }

            for (auto& temp_Info : temp[e_node]) {
                if (temp_Info.first == s_node) {
                    temp_Info.second = r_dist;
                    F2 = 1;
                    break;
                }
            }

            if (!F1) temp[s_node].push_back(make_pair(e_node, r_dist));
            if (!F2) temp[e_node].push_back(make_pair(s_node, r_dist));

            dijk(result);
            p_message(result, fp2);
            fp2.close();
        }

        cout << "Complete. Output file written to output_ls.txt." << endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "usage: linkstate topologyfile messagesfile changesfile" << endl;
        return 1;
    }

    int node_count = 0; 
    l_state ls(node_count); 
    ls.process(argv[1], argv[2], argv[3]);
    return 0;
}
