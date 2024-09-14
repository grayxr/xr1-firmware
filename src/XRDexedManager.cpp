#include <XRDexedManager.h>

namespace XRDexedManager
{
    uint8_t activeInstances[4] = {0, 1, 2, 3}; // Initialize active instances
    uint8_t inactiveInstances[4] = {4, 5, 6, 7}; // Initialize inactive instances

    void init()
    {
        // Initialize active and inactive instances
        for (uint8_t a = 0; a < 4; a++)
        {
            activeInstances[a] = a;
        }

        for (uint8_t i = 4; i < 8; i++)
        {
            inactiveInstances[i] = i;
        }
        
    }

    // get the inactive instance for the track in the current pattern
    uint8_t getInactiveInstanceForTrack(uint8_t track)
    {
        return inactiveInstances[track];
    }

    // swap active and inactive instances right before/when pattern starts
    void swapInstances()
    {
        for (uint8_t i = 0; i < 4; i++)
        {
            uint8_t temp = activeInstances[i];
            activeInstances[i] = inactiveInstances[i];
            inactiveInstances[i] = temp;
        }
    }

    // get the active instance for the track in the current pattern
    uint8_t getActiveInstanceForTrack(uint8_t track)
    {
        return activeInstances[track];
    }

} // namespace XRDexedManager