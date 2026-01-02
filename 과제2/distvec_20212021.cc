#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

int start;
int end_Node;
int node_count;
int f_start, f_end, f_cost;
int n_start, n_end, n_cost;

char message[1000];

void f_table(int*** area) {
    bool updated;

    do {
        updated = false;
        for (int i = 0; i < node_count; i++) {
            for (int j = 0; j < node_count; j++) {
                if (area[i][j][0] == j && area[i][j][1] != 0) {
                    for (int k = 0; k < node_count; k++) {
                        if (area[j][k][1] < 0) {
                            continue;
                        }
                        int n_distance = area[i][j][1] + area[j][k][1];
                        if (area[i][k][1] == -999 || n_distance < area[i][k][1]) {
                            if (n_distance < 0) {
                                continue;
                            }
                            area[i][k][0] = j;
                            area[i][k][1] = n_distance;
                            updated = true;
                        }
                    }
                }
            }
        }
    } while (updated);
}

void add_node(int*** d_table, int n_start, int n_end, int n_cost) {
    for (int i = 0; i < 2; ++i) {
        d_table[n_start][n_end][i] = (i == 0) ? n_end : n_cost;
        d_table[n_end][n_start][i] = (i == 0) ? n_start : n_cost;
    }
}


void s_Table(ofstream& out, int*** d_table) {
    int i = 0;
    while (i < node_count) {
        int j = 0;
        while (j < node_count) {
            if (d_table[i][j][1] != -999) {
                out << j << " " << d_table[i][j][0] << " " << d_table[i][j][1] << endl;
            }
            j++;
        }
        out << endl;
        i++;
    }

}

void p_message(ifstream& fp, ofstream& out, int*** d_table) {
    fp.clear();
    fp.seekg(0, ios::beg);
    while (fp >> start >> end_Node) {
        fp.getline(message, 1000);
        if (d_table[start][end_Node][1] != -999) {
            out << "from " << start << " to " << end_Node << " cost " << d_table[start][end_Node][1] << " hops ";
            int temp = start;
            while (temp != end_Node) {
                out << temp << " ";
                temp = d_table[temp][end_Node][0];
            }
            out << "message" << message<<endl;
        }
        else { 
            out << "from " << start << " to " << end_Node << " cost infinite hops unreachable message" << message;
        }
    }

    out << endl;
    fp.close();
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "usage: distvec topologyfile messagesfile changesfile" << endl;
        return 0;
    }

    ifstream fp1(argv[1]), fp2(argv[2]), fp3(argv[3]);
    ofstream result("output_dv.txt");

    if (!fp1 || !fp2 || !fp3 || !result) {
        cerr << "Error: open input file." << endl;
        return 0;
    }

    fp1 >> node_count;

    int*** d_table = new int** [node_count];
    for (int i = 0; i < node_count; ++i) {
        d_table[i] = new int* [node_count];
        for (int j = 0; j < node_count; ++j) {
            d_table[i][j] = new int[2];
            d_table[i][j][0] = (i != j) ? -1 : i;
            d_table[i][j][1] = (i != j) ? -999 : 0;
        }
    }

    while (fp1 >> f_start >> f_end >> f_cost) {
        for (int i = 0; i < 2; ++i) {
            d_table[f_start][f_end][i] = (i == 0) ? f_end : f_cost;
            d_table[f_end][f_start][i] = (i == 0) ? f_start : f_cost;
        }
    }


    fp1.close();

    f_table(d_table);
    if (!result) {
        cerr << "Error: open output file." << endl;
        return 0;
    }
    s_Table(result, d_table);
    p_message(fp2, result, d_table);

    while (true) {
        for (int i = 0; i < node_count; i++) {
            for (int j = 0; j < node_count; j++) {
                d_table[i][j][0] = (i == j) ? j : -1;
                d_table[i][j][1] = (i == j) ? 0 : -999;
            }
        }
        fp1.open(argv[1]);
        fp1 >> node_count;
        while (fp1 >> f_start >> f_end >> f_cost) {
            for (int i = 0; i < 2; ++i) {
                d_table[f_start][f_end][i] = (i == 0) ? f_end : f_cost;
                d_table[f_end][f_start][i] = (i == 0) ? f_start : f_cost;
            }
        }
        fp1.close();

        if (!(fp3 >> n_start >> n_end >> n_cost))
        {
            break;
        }

        add_node(d_table, n_start, n_end, n_cost);
        f_table(d_table);
        s_Table(result, d_table);

        fp2.open(argv[2]);
        p_message(fp2, result, d_table);
        fp2.close();
    }

    cout << "Complete. Output file written to output_dv.txt." << endl;

    fp3.close();
    result.close();
    for (int i = 0; i < node_count; ++i) {
        for (int j = 0; j < node_count; ++j) {
            delete[] d_table[i][j];
            d_table[i][j] = nullptr;
        }
        delete[] d_table[i];
        d_table[i] = nullptr;
    }
    delete[] d_table;
    d_table = nullptr;


    return 0;
}
