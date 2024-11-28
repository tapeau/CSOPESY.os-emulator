#include "IMemoryAllocator.h"
#include <cstddef>
#include <memory>
#include <unordered_map>
class PagingAllocator : public IMemoryAllocator
{
  public:
    PagingAllocator(size_t max_mem_size, size_t mem_per_frame);
    static PagingAllocator& getInstance();

    void* allocate(std::shared_ptr<Process> process) override;
    void deallocate(std::shared_ptr<Process> process) override;
    std::string visualizeMemory() override;
    int getPageIn() const;
    int getPageOut() const;
    
  private:
    PagingAllocator() = default;

    // **Delete copy constructor and assignment operator**
    // Prevents copying or assigning the Singleton instance.
    PagingAllocator(const PagingAllocator&) = delete;
    PagingAllocator& operator=(const PagingAllocator&) = delete;
    
    int pageIn = 0;
    int pageOut = 0;
    size_t max_mem_size;
    size_t num_frame;
    std::unordered_map<size_t, size_t> frames;
    std::vector<size_t> free_frames;
    size_t mem_per_frame; // since memory per process is pre-determined
    std::mutex mem_mtx;

    size_t allocFrames(size_t num_frame, size_t pid);
    void deallocFrames(size_t num_frame, size_t frame_index);
    
};
