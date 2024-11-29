#include "PagingAllocator.h"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>

PagingAllocator::PagingAllocator(size_t max_mem_size, size_t mem_per_frame)
  : max_mem_size(max_mem_size), mem_per_frame(mem_per_frame)
{
  num_frame = max_mem_size / mem_per_frame;

  // Init free frame list
  // std::cout << "Initial free frames: ";
  for (size_t i = 0; i < num_frame; ++i) {
    free_frames.push_back(i);
    // std::cout << free_frames[i] << " ";
  }
}

size_t PagingAllocator::allocFrames(size_t num_frame, size_t pid)
{

  size_t frame_index = free_frames.back();
  free_frames.pop_back();

  for (size_t i = 0; i < num_frame; ++i) {
    // std::cout << "Current Val: " << frames[frame_index + i] << " @ " << frame_index + i << "\n";
    frames[frame_index + i] = pid;
    // std::cout << "Assigning " << frame_index + i << " to pid: " << pid << " Updated Val: " << frames[frame_index + i] << "\n"; 
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
  std::lock_guard<std::mutex> lock(mem_mtx);
  if (!process->isAllocated()) {
    size_t process_id = process->getPID();
    // size_t proc_frames_need = process->getMemReq() / mem_per_frame; 
    size_t proc_frames_need = (process->getMemReq() + mem_per_frame - 1) / mem_per_frame;
    // std::cout << process_id << ": " << "Allocating " << proc_frames_need << " in Memory\n";
    if (proc_frames_need > free_frames.size()) {
      // std::cerr << "Memory allocation failed. Not enough free frames.\n";
      // std::cout << "Cannot allocate" << ": " << process->getName() << " Not enough frames.\n";
      return nullptr;
    }
    // DEBUGGING
    // std::cout << "Before allocation: \n";
    // std::cout << "Free frames (before): ";
    // for (const auto& frame : free_frames) {
    //   std::cout << frame << " ";
    // }
    // std::cout << "\n";
    // for (const auto& frame : frames) {
    //   std::cout << frame.first << ": " << frame.second << "\n";
    // }
    // Allocate frames for the process
    size_t frame_index = allocFrames(proc_frames_need, process_id);
    page_in += proc_frames_need;
    process->memAlloc(0, 1); // just to signify that process is allocated
    // DEBUGGING
    // std::cout << "Successfully allocated " << process->getName() << " with " << proc_frames_need << " in Memory\n";
    // std::cout << "After allocation: \n";
    // std::cout << "Free frames (after): ";
    // for (const auto& frame : free_frames) {
    //   std::cout << frame << " ";
    // }
    // std::cout << "\n";
    // for (const auto frame : frames) {
    //   std::cout << frame.first << ": " << frame.second << "\n";
    // }
    alloc_size += process->getMemReq();
    return reinterpret_cast<void*>(frame_index);
  } 
  return nullptr;
}

size_t PagingAllocator::getPageIn() const
{
  return page_in;
}

size_t PagingAllocator::getPageOut() const 
{
  return page_out;
}

void PagingAllocator::deallocate(std::shared_ptr<Process> process)
{
  std::scoped_lock lock{mem_mtx};
  size_t proc_id = process->getPID();

    auto it = std::find_if(frames.begin(), frames.end(),
        [proc_id](const auto& entry) { return entry.second == proc_id; });

    // std::cout << "Deallocating " << process->getName() << " Status: " << process->hasFinished() << "\n";

    while (it != frames.end()) {
      size_t frame_index = it->first;
      deallocFrames(1, frame_index);
      it = std::find_if(frames.begin(), frames.end(), 
          [proc_id](const auto& entry) { return entry.second == proc_id; });
    }
    // std::cout << "After deallocation: \n";
    // std::cout << "Free frames (after): ";
    // for (const auto& frame : free_frames) {
    //   std::cout << frame << " ";
    // }
    // std::cout << "\n";
    // for (const auto frame : frames) {
    //   std::cout << frame.first << ": " << frame.second << "\n";
    // }
    size_t proc_frames_need = (process->getMemReq() + mem_per_frame - 1) / mem_per_frame;
    page_out += proc_frames_need;
    alloc_size -= process->getMemReq();
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

size_t PagingAllocator::getMem() const
{
  return alloc_size;
}
