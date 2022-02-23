#include "EventQueue.hpp"
#include <memory>

CEventQueue::CEventQueue(size_t iCapacity) : fCapacity(iCapacity), fHeadIndex(0)
{
    fEventBank = (SEvent*) malloc(sizeof(SEvent) * iCapacity);
}

bool CEventQueue::Push(SEvent iEvent) {
    if (fHeadIndex >= fCapacity - 1) 
        return false;
    
    
    fEventBank[fHeadIndex] = iEvent;
    fHeadIndex++;

    return true;
}

bool CEventQueue::Pop(SEvent& oEvent) {
    if (fHeadIndex == 0) return false;

    fHeadIndex--;
    oEvent = fEventBank[fHeadIndex];

    return true;
}