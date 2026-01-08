#include "FlickInputSettings.h"

FFlickInputSettings::FFlickInputSettings()
{
}


void FFlickInputSettings::ForwardInput(float value)
{
    axisValue = value;
    
    if (bFlickPossible)
    {
        if(bFirstUpThenDown)
        {
            if(IsValueUp(value) && !bReachedDown)
            {
                bReachedUp = true;
            //    GlobalTimeWhenInputStarted = GWorld->GetTimeSeconds();
            }
            if(IsValueDown(value) && bReachedUp) 
            {
                bReachedDown = true;
            }
        }
        else
        {
            if(IsValueDown(value) && !bReachedUp)
            {
                bReachedDown = true;
           //     GlobalTimeWhenInputStarted = GWorld->GetTimeSeconds();
            }
            if(IsValueUp(value) && bReachedDown)
            {
                bReachedUp = true;
            }
        }
    }

    // If the flick is performed
    if(bReachedUp && bReachedDown)
    {
        Reset();

        // Broadcast the event when flick is performed
        OnFlickPerformed.Broadcast(StuntEnum);
        GlobalTimeWhenInputStarted = 0;
        // Reset the flick state and stop further input processing
    }
}

void FFlickInputSettings::Reset()
{
    // Reset the flick tracking state
    bReachedUp = false;
    bReachedDown = false;

    //elapsedTime = 0.0f;
}

void FFlickInputSettings::Tick(float DeltaTime)
{
    if (GWorld->GetTimeSeconds() - GlobalTimeWhenInputStarted < TimeToPerformFlick)
    {
        bFlickPossible = true;
    }
    else
    {
        bFlickPossible = false;
    }
    if ((axisValue < 0.5f && axisValue > -0.5f))
    {
        if (!bFlickPossible || (bFlickPossible && !bReachedDown && !bReachedUp))
        {
            Reset();
            GlobalTimeWhenInputStarted = GWorld->GetTimeSeconds();
        }
    }

    axisValue = 0;  
}

bool FFlickInputSettings::IsValueUp(const float value)
{
    return value >= 0.75f;
}

bool FFlickInputSettings::IsValueDown(const float value)
{
    return value <= -0.75f;
}
