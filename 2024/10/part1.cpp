#include <algorithm>
#include <limits>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

constexpr size_t kMaxSize = numeric_limits<size_t>::max();
constexpr size_t kEmptySpace = kMaxSize;

struct Block
{
    size_t fileId;
    size_t address;
    size_t size;
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
    vector<Block> memoryMap;
    vector<Block> freeBlocks;
    size_t heapPointer;
    
    Memory() : heapPointer(0)
    {
    }
    
    void alloc(size_t fileId, size_t fileSize)
    {
        Block block = {fileId, heapPointer, fileSize};
        if (fileId == kEmptySpace)
        {
            freeBlocks.push_back(block);
        }
        else
        {
            memoryMap.push_back(block);
        }
        heapPointer += fileSize;
    }
    
    void compress()
    {
        size_t freeBlockIndex = 0;
        size_t fileBlockIndex = memoryMap.size() - 1;
        
        while (fileBlockIndex < kMaxSize && freeBlockIndex < freeBlocks.size() &&
               freeBlocks[freeBlockIndex].address < memoryMap[fileBlockIndex].address)
        {
            auto& fileBlock = memoryMap[fileBlockIndex];
            auto& freeBlock = freeBlocks[freeBlockIndex];
            size_t allocatableSize = min(fileBlock.size, freeBlock.size);
            
            fileBlock.size -= allocatableSize;
            
            Block newBlock = {fileBlock.fileId, freeBlock.address, allocatableSize};
            
            freeBlock.size -= allocatableSize;
            freeBlock.address += allocatableSize;
            
            if (freeBlock.size == 0)
            {
                ++freeBlockIndex;
            }
            if (fileBlock.size == 0)
            {
                --fileBlockIndex;
            }
            memoryMap.push_back(newBlock);
        }
        
        tidyBlocks(memoryMap);
        heapPointer = memoryMap.back().address + memoryMap.back().size;
        tidyBlocks(freeBlocks, heapPointer);
        
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