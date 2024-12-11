#include <algorithm>
#include <limits>
#include <iostream>
#include <vector>
#include <queue>
#include <string>
using namespace std;

constexpr size_t kMaxSize = numeric_limits<size_t>::max();
constexpr size_t kEmptySpace = kMaxSize;

struct Block
{
    size_t fileId;
    size_t address;
    size_t size;
    
    bool operator>(const Block& other) const
    {
        return address > other.address;
    }
};

size_t sumNToM(size_t n, size_t m)
{
    return (m*(m+1) - n*(n-1))/2;
}

void tidyBlocks(vector<Block>& blocks, size_t memorySize = kMaxSize)
{
    vector<Block> tidyBlocks;
    tidyBlocks.reserve(blocks.size());
    for (const auto& block : blocks)
    {
        if (block.size && block.address < memorySize)
        {
            tidyBlocks.push_back(block);
        }
    }
    sort(tidyBlocks.begin(), tidyBlocks.end(), [](const Block& b1, const Block& b2)
    {
        return b1.address < b2.address;
    });
    blocks = move(tidyBlocks);
}

struct Memory
{
    typedef priority_queue<Block,vector<Block>,greater<Block>> BlockQueue;
    
    vector<Block> memoryMap;
    vector<BlockQueue> freeBlocks;
    size_t heapPointer;
    
    Memory() : freeBlocks(10), heapPointer(0)
    {
    }
    
    void alloc(size_t fileId, size_t fileSize)
    {
        Block block = {fileId, heapPointer, fileSize};
        if (fileId == kEmptySpace)
        {
            freeBlocks[block.size].push(block);
        }
        else
        {
            memoryMap.push_back(block);
        }
        heapPointer += fileSize;
    }
    
    Block popLeftmostFreeBlock(size_t minimumSize, size_t leftOf)
    {
        Block ans{kEmptySpace, kMaxSize, 0};
        for (size_t size = minimumSize; size <= 9; ++size)
        {
            if (!freeBlocks[size].empty() && freeBlocks[size].top().address < min(leftOf, ans.address))
            {
                ans = freeBlocks[size].top();
            }
        }
        if (ans.address != kMaxSize)
        {
            if (ans.size > minimumSize)
            {
                Block newFreeBlock = {kEmptySpace, ans.address+minimumSize, ans.size - minimumSize};
                freeBlocks[newFreeBlock.size].push(newFreeBlock);
            }
            freeBlocks[ans.size].pop();
        }
        return ans;
    }
    
    void compress()
    {
        size_t fileBlockIndex = memoryMap.size()-1;
        
        while (fileBlockIndex < kMaxSize)
        {
            auto& fileBlock = memoryMap[fileBlockIndex];
            
            Block freeBlock = popLeftmostFreeBlock(fileBlock.size, fileBlock.address);
            
            if (freeBlock.address != kMaxSize)
            {
                fileBlock.address = freeBlock.address;
            }
            
            --fileBlockIndex;
        }
        
        tidyBlocks(memoryMap);
        heapPointer = memoryMap.back().address + memoryMap.back().size;
    }
    
    size_t calculateChecksum() const
    {
        size_t checksum = 0;
        for (const auto& block : memoryMap)
        {
            checksum += block.fileId*sumNToM(block.address, block.address+block.size-1);
        }
        return checksum;
    }
    
    void display(ostream& out) const
    {
        size_t previousAddress = 0;
        for (const auto& block : memoryMap)
        {
            for (size_t i = previousAddress; i < block.address; ++i)
            {
                out << '.';
            }
            for (size_t i = 0; i < block.size; ++i)
            {
                out << block.fileId;
            }
            previousAddress = block.address+block.size;
        }
        out << '\n';
    }
};

int main()
{
    Memory memory;
    
    string memoryDump;
    cin >> memoryDump;
    
    size_t fileId = 0;
    bool empty = false;
    for (char c : memoryDump)
    {
        if (empty)
        {
            memory.alloc(kEmptySpace, c-'0');
        }
        else
        {
            memory.alloc(fileId++, c-'0');
        }
        empty = !empty;
    }
    
    memory.compress();
    cout << memory.calculateChecksum() << endl;
    
    memory.display(cout);
}