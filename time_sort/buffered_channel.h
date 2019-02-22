#pragma once

#include <deque>
#include "common/common_types.h"

class TTree;

constexpr u64 max_time = std::numeric_limits<u64>::max();
constexpr std::size_t buffer_size = 10000;

class BufferedChannel {
public:
    BufferedChannel();
    BufferedChannel(TTree* ch_tree);
    
    void Next();
    void Refresh();
    
    u64 GetTime() const;
    u64 GetLonggate() const;
    u64 GetShortgate() const;
    bool Finished() const;
    
private:
    std::size_t current_entry;
    std::size_t n_entries;
    u64 time;
    u64 longgate;
    u64 shortgate;
    TTree* tree;
    bool finished;
    
    std::size_t events_in_buffer;
    std::deque<u64> buffered_time;
    std::deque<u64> buffered_shortgate;
    std::deque<u64> buffered_longgate;
};
