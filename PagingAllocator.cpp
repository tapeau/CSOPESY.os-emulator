#include "PagingAllocator.h"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>


PagingAllocator::PagingAllocator(size_t max_mem_size, size_t mem_per_frame)
  : max_mem_size(max_mem_size) {

    num_frame = max_mem_size / mem_per_frame;
    this->mem_per_frame = mem_per_frame;

    // Init free frame list
    for (size_t i = 0; i < num_frame; ++i) {
      free_frames.push_back(i);
    }
  }

size_t PagingAllocator::allocFrames(size_t num_frame, size_t pid)
{
  size_t frame_index = free_frames.back();
  free_frames.pop_back();

  for (size_t i = 0; i < num_frame; ++i) {
    frames[frame_index + i] = pid;
  
  }
  return frame_index;
}

void PagingAllocator::deallocFrames(size_t num_frame, size_t frame_index)
{
  for (size_t i = 0; i < num_frame; ++i) {
    frames.erase(frame_index + i);
  }

  for (size_t i = 0; i < num_frame; ++i) {
    free_frames.push_back(frame_index + i);
  }
}

void* PagingAllocator::allocate(std::shared_ptr<Process> process)
{
  size_t process_id = process->getPID();
  size_t num_frames_need; 
  // size_t frames_needed = process;
  if (num_frames_need > free_frames.size()) {
    std::cerr << "Memory allocation failed. Not enough frames.\n";
  return nullptr;
  }
  // size_t frame_index = allocate
  return nullptr;
}

void PagingAllocator::deallocate(std::shared_ptr<Process> process)
{
  size_t pid = process->getPID();

  auto it = std::find_if(frames.begin(), frames.end(),
      [pid](const auto& entry) { return entry.second == pid; });

  while (it != frames.end()) {
    size_t frame_index = it->first;
    deallocFrames(1, frame_index);
  }
}