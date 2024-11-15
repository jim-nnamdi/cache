#include <iostream>
#include <list>
#include <string>
#include <fstream>
#include <memory>
#include <algorithm>
#include <unordered_map>

#define NEG_FS -1
#define SM_CONTENT "BASE_FILE"

class fs_nodes {
    public:
    std::string key;
    fs_nodes* nextfile;
    fs_nodes* prevfile;
    fs_nodes(std::string fkey){
        key = fkey;
        nextfile = nullptr; prevfile = nullptr;
    }
};

class fs_cache{
    private:
    size_t fs_capacity;
    std::unordered_map<std::string, 
        fs_nodes*> cache;
    fs_nodes* fs_top;
    fs_nodes* fs_bottom;
    
    void remove_fs(fs_nodes* fs) {
        fs_nodes* PF_Nodes = fs->prevfile;
        fs_nodes* NF_Nodes = fs->nextfile;
        PF_Nodes->nextfile = NF_Nodes;
        NF_Nodes->prevfile = PF_Nodes;
    }

    void add_fs(fs_nodes* fs) {
        fs->nextfile = fs_top->nextfile;
        fs->prevfile = fs_top;
        fs_top->nextfile->prevfile = fs;
        fs_top->nextfile = fs;
    }

    void move_fs(fs_nodes* fs) {
        remove_fs(fs);
        add_fs(fs);
    }

    public:
    fs_cache(size_t cap) {
        fs_capacity = cap;
        fs_top = new fs_nodes(std::string());
        fs_bottom = new fs_nodes(std::string());
        fs_top->nextfile = fs_bottom;
        fs_bottom->prevfile = fs_top;
    }

    std::string get_fs(std::string fs_key) {
        if(cache.find(fs_key) != cache.end()){
            fs_nodes* node = cache[fs_key];
            move_fs(node);
            return node->key;
        } 
        std::string NEG_str = std::to_string(NEG_FS);
        std::ifstream sample_file(fs_key);
        if(!sample_file.is_open())perror("error");
        std::string contents((std::istreambuf_iterator<char>(sample_file)), 
        std::istreambuf_iterator<char>());
        sample_file.close();
        return contents;
    }

    std::string put_fs(std::string fs_key) {
        if(cache.find(fs_key) != cache.end()){
            fs_nodes* node = cache[fs_key];
            cache[fs_key] = node;
            move_fs(node);
            return fs_key;
        } else {
            if(cache.size() == fs_capacity) {
                fs_nodes* least_accessed = fs_bottom->prevfile;
                remove_fs(least_accessed);
                cache.erase(least_accessed->key);
                delete(least_accessed);
            }

            fs_nodes* file_node = new fs_nodes(fs_key);
            cache[fs_key] = file_node;
            add_fs(file_node);
            std::ofstream new_sample(fs_key);
            if(!new_sample.is_open())perror("error");
            new_sample << SM_CONTENT;
            new_sample.close();
            return file_node->key;
        }
    }
};