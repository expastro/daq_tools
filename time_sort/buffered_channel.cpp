#include <iostream>
#include <TTree.h>
#include "buffered_channel.h"

BufferedChannel::BufferedChannel() {}

BufferedChannel::BufferedChannel(TTree* ch_tree)
: tree(ch_tree) {
    tree->SetBranchAddress("t", &time);
    tree->SetBranchAddress("longgate", &longgate);
    tree->SetBranchAddress("shortgate", &shortgate);
    tree->GetEntry(0);
    current_entry=0;
    n_entries = tree->GetEntries();
    events_in_buffer=0;
    Refresh();
    finished=false;
}

void BufferedChannel::Next() {
    if (finished) {
        return;
    }
    buffered_time.pop_front();
    buffered_shortgate.pop_front();
    buffered_longgate.pop_front();
    events_in_buffer--;
    if(events_in_buffer==0) {
        Refresh();
    }
    if (buffered_time.front() == max_time) {
        finished = true;
    }
}

void BufferedChannel::Refresh(){
    while (events_in_buffer < buffer_size && current_entry <= n_entries) {
        tree->GetEntry(current_entry);
        buffered_time.push_back(time);
        buffered_shortgate.push_back(shortgate);
        buffered_longgate.push_back(longgate);
        events_in_buffer++;
        current_entry++;
    }
    
    if (events_in_buffer < buffer_size )
    {
        std::cout << std::endl << "All Events of " << tree->GetName() << " were loaded into Buffer! " << std::endl;
        buffered_time.push_back(max_time);
        buffered_shortgate.push_back(0);
        buffered_longgate.push_back(0);
        events_in_buffer++;
        current_entry = n_entries;
    }
}

u64 BufferedChannel::GetTime() const {
    return buffered_time.front();
}

u64 BufferedChannel::GetLonggate() const {
    return buffered_longgate.front();
}

u64 BufferedChannel::GetShortgate() const {
    return buffered_shortgate.front();
}

bool BufferedChannel::Finished() const  {
    return finished;
}
