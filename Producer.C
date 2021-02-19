#include <stdio.h>

#include "Producer.h"
#include <time.h>

Producer::Producer()
{
    timeToProduceItem = 0;
    isFrogBite = false;

    sharedBeltData = nullptr;

    frogsProduced = 0;
    escargotProduced = 0;
}

Producer::Producer(unsigned int productionTime, bool frog, Belt* belt)
{
    timeToProduceItem = productionTime;
    isFrogBite = frog;

    sharedBeltData = belt;

    frogsProduced = 0;
    escargotProduced = 0;
}

void Producer::produceItem()
{
    // Create timespec struct and set delay
    struct timespec sleepTime;

    sleepTime.tv_sec = timeToProduceItem / MS_PER_SEC;
	sleepTime.tv_nsec = (timeToProduceItem % MS_PER_SEC) * NS_PER_MS;

    // Produce items until the specified number of items has been produced
    while (sharedBeltData->numberOfItemsProduced <
    sharedBeltData->totalNumberOfItems)
    {
        // Sleep before next item is produced
        nanosleep(&sleepTime, NULL);

        // If we're producing frog bites, then wait the corresponding semaphore
        if (isFrogBite)
        {
            sem_wait(&sharedBeltData->availableFrogTreatSpaces);
        }

        // Enter critical section
        sem_wait(&sharedBeltData->availableTreatSpaces);
        sem_wait(&sharedBeltData->mutex);

        if (sharedBeltData->numberOfItemsProduced <
        sharedBeltData->totalNumberOfItems)
        {
            // Produce item and add it to the next spot on the array
            // Also update variables in respective classes
            if (isFrogBite)
            {
                sharedBeltData->itemsOnBelt.push(frogValue);

                frogsProduced++;

                sharedBeltData->numberOfItemsProduced += 1;
                sharedBeltData->numOfFrogsOnBelt += 1;
            }
            else
            {            
                sharedBeltData->itemsOnBelt.push(escargotValue);

                escargotProduced++;

                sharedBeltData->numberOfItemsProduced += 1;
                sharedBeltData->numOfEscargotOnBelt +=1;
            }

            // Output that an item was produced
            cout << "Belt: " << sharedBeltData->numOfFrogsOnBelt << " frogs + "
            << sharedBeltData->numOfEscargotOnBelt << " escargot = " <<
            (sharedBeltData->numOfFrogsOnBelt +
            sharedBeltData->numOfEscargotOnBelt)
            << ". Produced: " << sharedBeltData->numberOfItemsProduced << "\t";
            if (isFrogBite)
            {
                cout << "Added crunchy frog bite." << endl;
            }
            else
            {
                cout << "Added escargot sucker." << endl;
            }
        }

        // There is another item to consume, so update  sempahore
        sem_post(&sharedBeltData->itemsToConsume);

        // Release the hold on the critical area
        sem_post(&sharedBeltData->mutex);
    }   
}