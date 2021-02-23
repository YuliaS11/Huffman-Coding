#include <fstream>
#include <string>
#include <thread>
#include "storage.h"

using namespace std;

void main_out(const string &file, const string &fileinfo, const string & filecomp) {
    ifstream ifs;
    ofstream ofs;
    ifs.open(file, ios::in | ios::binary);
    unsigned char x = 0; // Input byte
    unique_ptr<Binary_Storage> bs = make_unique<Binary_Storage> ();
    while (ifs) {
        ifs.read (reinterpret_cast<char *>(&x), 1);
        bs->increment_byte(x);
    }
    ifs.close();

    bs->build_tree();

    ifs.open(file, ios::in | ios::binary);
    ofs.open(filecomp, ios::out | ios::binary | ios::trunc);
    unsigned long long bit_count = 0; // Total number of bits
    unsigned char count = 0, y = 0; // Currently loaded number of bits.
    while (ifs) {
        ifs.read(reinterpret_cast<char *>(&x), 1);
        vector<unsigned char> & path = bs->get_byte_path(x);
        bit_count += path.size();
        for (auto bit: path) {
            y = (y << 1) | bit;
            count++;
            if (count >> 3) {
                count = 0;
                ofs.write(reinterpret_cast<char *>(&y),1);
                y = 0;
            }
        }
    }
    ifs.close();
    ofs.close();
    
    ofs.open(fileinfo, ios::out | ios::trunc);
    ofs << bit_count << endl;
    bs->print_tree(ofs);
    //bs->print_tree(cout);
    ofs.close();
}

void main_in(const string & file, const string & fileinfo,
                const string & filecomp, const string & filedeco) {
    ifstream ifs;
    ofstream ofs_temp, ofs_tree, ofstream, ofs_deco;

    ifs.open(fileinfo, ios::in);
    unique_ptr<Simple_Tree> st = make_unique<Simple_Tree> ();
    unsigned long long bit_count;
    ifs >> bit_count;
    bit_count--;
    unsigned short num;
    while (ifs) {
        num = 257;
        ifs >> num;
        if (num == 257) break;
        if (num == 256) { st->load_node(); }
        else { st->load_node(num); }
    }
    ifs.close();
    st->clear_up();
    //st->print_tree(cout);

    ifs.open(filecomp, ios::in);
    ofs_deco.open(filedeco, ios::out | ios::binary | ios::trunc);
    unsigned long cur_bit = 0;
    unsigned char x = 0, y = 0, loaded = 8; // Input and Output byte, and current number of loaded bits;
    ifs.read(reinterpret_cast<char *>(&x),1);
    while (ifs && cur_bit < bit_count) {
        loaded--;
        cur_bit++;
        if ((x & (1 << loaded))) { st->move_right(); }
        else { st->move_left(); }
        if (st->at_leaf()){
            y = st->get_byte();
            ofs_deco.write(reinterpret_cast<char *>(&y),1);
        }
        if (!loaded) { ifs.read(reinterpret_cast<char *>(&x),1); loaded = 8; }
    }
    ifs.close();
    ofs_deco.close();
}

int comp(const string & file, const string & filedeco) {
    ifstream ifs, ifs_deco;
    ifs.open(file, ios::in | ios::binary);
    ifs_deco.open(file, ios::in | ios::binary);
    char byte = 0;
    char byte_deco = 0;
    while (ifs && ifs_deco) {
        ifs.read(&byte, 1);
        ifs_deco.read(&byte_deco, 1);
        if (byte != byte_deco) return 1;
    }
    return 0;
}

string get_deco(const string &file) {
    string filedir, filename, fileext;
    size_t idxdir = file.find_last_of("/\\");
    if (idxdir == string::npos) {
        idxdir = 0;
        filedir = "";
    }
    else { filedir = file.substr(0, idxdir+1); }
    size_t idxext = file.find_first_of('.', idxdir+1);
    if (idxext == string::npos) {
        idxext = 0;
        fileext = "";
    }
    else { fileext = file.substr(idxext); }
    if (!idxdir && !idxext) {
        filename = file;
    }
    else if (!idxdir) { filename = file.substr(0, idxext); }
    else if (!idxext) { filename = file.substr(idxdir+1); }
    else { filename = file.substr(idxdir+1, idxext - idxdir -1);}
    return filedir + filename + "_deco" + fileext;
}

void main_thread(const string & file, bool verify, bool generate, bool skip, bool * global_error) {
    string fileinfo = file + "_info";
    string filecomp = file + "_comp";
    string filedeco = get_deco(file);

    ifstream ifs(file);
    if((!generate || verify) && ifs.fail()) {
        cerr << "File " << file << " cannot be opened." << endl;
        *global_error = true;
        return;
    }
    ifs.close();

    bool error;

    error = false;
    if (!skip) { main_out(file, fileinfo, filecomp); }
    else {
        ifstream ifs_comp(filecomp);
        ifstream ifs_tree(fileinfo);
        if (ifs_comp.fail()) {
            cerr << "Compression file "
                << filecomp << " cannot be opened." << endl;
            error = true;
        }
        if (ifs_tree.fail()) {
            cerr << "Info file "
                << fileinfo << " cannot be opened." << endl;
            error = true;
        }
        ifs_comp.close();
        ifs_tree.close();
        if (error) { *global_error = true; return; }
    }

    error = false;
    if (verify || generate) {
        main_in(file, fileinfo, filecomp, filedeco);
        if (verify) { if (comp(file, filedeco)) error = true; }
    }
    if (!generate) { remove(filedeco.c_str()); }
    if (error) { 
        cerr << "Provided " << filecomp << "is faulty." << endl;
        *global_error = true;
    }
}

int main(int argc, char * argv[]) {
    cout.flush();
    cerr.flush();
    vector<string> files;
    vector<thread> threads;
    if (argc == 0) {
        cerr << "Must specify a file." << endl;
        return 1;
    }
    string option;
    bool verify = false, skip = false, generate = false;
    for (int i = 1; i < argc; i++) {
        option = argv[i];
        if (option == "-v" || option == "--verify") {
            verify = true;
        } // Verify if file is fine
        else if (option == "-s" || option == "--skip") {
            skip = true;
        } // Skip generating the compression file
        else if (option == "-g" || option == "--generate") {
            generate = true;
        }
        else {
            files.emplace_back(option);
        }
    }
    bool global_error = false;
    for (auto file: files) {
        threads.emplace_back(thread(main_thread, file, verify, generate, skip, &global_error));
        //main_thread(file, verify, generate, skip, &global_error);
    }
    for (auto & th: threads) {
        th.join();
    }
    if (global_error) return 1;
}
