#include <chrono>
#include <fmt/format.h>
#include <iostream>
#include <limits>
#include <memory>
#include <TFile.h>
#include <TTree.h>
#include "common/common_types.h"
#include "buffered_channel.h"

static bool EventsAvailable(const std::map<std::size_t, BufferedChannel>& channel_map) {
    for (const auto& channel : channel_map) {
        if (!channel.second.Finished())
        return true;
    }
    return false;
}

void SortTime(const std::string& filename) {
    // Open old File and Tree
    TFile input(filename.c_str(),"READ");
    TTree* input_tree = static_cast<TTree*>(input.Get("h509"));
    
    // TODO: Get number_of_channels
    const std::size_t number_of_channels = 16;
    std::string output_rootfile = fmt::format("{}_raw.root", filename.substr(0, filename.size() - 5));
    
    // Open new file and create trees:
    TFile output(output_rootfile.c_str(),"RECREATE");
    std::array<TTree*, number_of_channels> new_trees;
    for (std::size_t k=0; k < number_of_channels; k++) {
        std::string tree_name = fmt::format("Channel_{:02d}",k);
        std::string condition = fmt::format("ch=={}",k);
        new_trees[k]= input_tree->CopyTree(condition.c_str());
        new_trees[k]->SetName(tree_name.c_str());
        new_trees[k]->Write();
    }
    
    // Make empty output tree;
    std::string final_rootfile = fmt::format("{}_sorted.root", filename.substr(0, filename.size() - 5));
    TFile final_file(final_rootfile.c_str(),"RECREATE");
    TTree* out_tree = new TTree("h509", "h509");
    
    u64 ch_number;
    u64 time;
    u64 longgate;
    u64 shortgate;
    out_tree->Branch("ch",&ch_number,"ch/l");
    out_tree->Branch("t",&time,"t/l");
    out_tree->Branch("longgate",&longgate,"longgate/l");
    out_tree->Branch("shortgate",&shortgate,"shortgate/l");
    
    std::map<std::size_t, BufferedChannel> channels;
    std::vector<u64> time_vec;
    for (std::size_t k = 0; k < number_of_channels; k++) {
        channels.emplace(k,BufferedChannel(new_trees[k]));
        time_vec.push_back(channels[k].GetTime());
    }
    
    std::size_t currentEvent(0);
    auto lastPrintTime = std::chrono::steady_clock::now();
    
    while(EventsAvailable(channels)) {
        u64 next_channel = std::distance(time_vec.begin(), std::min_element(time_vec.begin(), time_vec.end()));
        ch_number = next_channel;
        time = channels[next_channel].GetTime();
        longgate = channels[next_channel].GetLonggate();
        shortgate = channels[next_channel].GetShortgate();
        if (time != max_time) {
            out_tree->Fill();
        }
        channels[ch_number].Next();
        time_vec[ch_number]= channels[ch_number].GetTime();
        
        if ((std::chrono::steady_clock::now() - lastPrintTime).count() * 1000 >=  + 50) {
            std::cout << fmt::format("\rAnalyzing event {}", currentEvent) << std::flush;
            lastPrintTime = std::chrono::steady_clock::now();
        }
        ++currentEvent;
    }
    final_file.Write();
}
