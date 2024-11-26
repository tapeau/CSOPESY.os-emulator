#include "PagingAllocator.h"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>


PagingAllocator::PagingAllocator(size_t max_mem_size, size_t mem_per_frame)
  : max_mem_size(max_mem_size) 
{
  // max_mem_size
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
  std::scoped_lock lock{mem_mtx};
  if (process->isAllocated()) {
    return nullptr;
  }
  size_t process_id = process->getPID();
  size_t num_frames_need = process->getMemReq() / mem_per_frame; 
  // std::cout << process_id << ": " << "Allocating " << num_frames_need << " in Memory\n";
  if (num_frames_need > free_frames.size()) {
    // std::cerr << "Memory allocation failed. Not enough free frames.\n";
    return nullptr;
  }

  // Allocate frames for the process
  size_t frame_index = allocFrames(num_frames_need, process_id);
  process->memAlloc(0, 1);
  std::cout << "Successfully allocated " << process_id << " with " << num_frames_need << " in Memory\n";
  return reinterpret_cast<void*>(frame_index);
}

void PagingAllocator::deallocate(std::shared_ptr<Process> process)
{
  size_t proc_id = process->getPID();

  auto it = std::find_if(frames.begin(), frames.end(),
      [proc_id](const auto& entry) { return entry.second == proc_id; });

  // size_t frames_used = process->getMemReq() / mem_per_frame;
  while (it != frames.end()) {
    size_t frame_index = it->first;
    // TODO: have to save number of pages needed somewhere in the process
    deallocFrames(1, frame_index);
    it = std::find_if(frames.begin(), frames.end(), 
        [proc_id](const auto& entry) { return entry.second == proc_id; });
  }
  process->memDealloc();
}

std::string PagingAllocator::visualizeMemory() 
{
  std::stringstream str_stream;
  str_stream << "------------------------------\n";
  str_stream  << "Memory Visualization: \n";
  for (size_t frame_index = 0; frame_index < num_frame; ++frame_index) {
    auto it = frames.find(frame_index);
    if (it != frames.end()) {
      str_stream << "Frame " << frame_index << " -> Process " << it->second << "\n";
    } else {
      str_stream << "Frame " << frame_index << " -> Free\n";
    }
  }
  str_stream << "------------------------------\n";
  return str_stream.str();
}


