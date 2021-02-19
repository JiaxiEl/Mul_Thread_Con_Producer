#include <stdio.h>
#include "Consumer.h"
#include <time.h>

Consumer::Consumer()
{
    consumerName = "";
    timeToConsumeItem = 0;

    sharedBeltData = nullptr;

    frogsConsumed = 0;
    escargotConsumed = 0;
}

Consumer::Consumer(string name, unsigned int consumptionTime, Belt* belt)
{
    consumerName = name;
    timeToConsumeItem = consumptionTime;

    sharedBeltData = belt;

    frogsConsumed = 0;
    escargotConsumed = 0;
}

void Consumer::consumeItem()
{
    // Create timespec struct and set delay
    struct timespec sleepTime;

    sleepTime.tv_sec = timeToConsumeItem / MS_PER_SEC;
	sleepTime.tv_nsec = (timeToConsumeItem % MS_PER_SEC) * NS_PER_MS;

    // Keep consuming items until the specified number of items is consumed
    while (sharedBeltData->numberOfItemsConsumed !=
    sharedBeltData->totalNumberOfItems
    || (!sharedBeltData->itemsOnBelt.empty()))
    {
        // Enter critical section
        sem_wait(&sharedBeltData->itemsToConsume);
        sem_wait(&sharedBeltData->mutex);

        if (sharedBeltData->numberOfItemsProduced !=
        sharedBeltData->totalNumberOfItems
        || (!sharedBeltData->itemsOnBelt.empty()))
        {
            // Frog
            if (sharedBeltData->itemsOnBelt.front() == frogValue)
            {
                frogsConsumed += 1;

                sharedBeltData->numberOfItemsConsumed += 1;
                sharedBeltData->numOfFrogsOnBelt -= 1;
            }
            // Escargot
            else if (sharedBeltData->itemsOnBelt.front() == escargotValue)
            {
                escargotConsumed += 1;

                sharedBeltData->numberOfItemsConsumed += 1;
                sharedBeltData->numOfEscargotOnBelt -= 1;
            }

            // Output that an item was consumed
            cout << "Belt: " << sharedBeltData->numOfFrogsOnBelt << " frogs + "
            << sharedBeltData->numOfEscargotOnBelt << " escargot = " <<
            (sharedBeltData->numOfFrogsOnBelt +
            sharedBeltData->numOfEscargotOnBelt)
            << ". Produced: " << sharedBeltData->numberOfItemsProduced << "\t";

            if (sharedBeltData->itemsOnBelt.front() == frogValue)
            {
                cout << consumerName << " consumed: crunchy frog bite." << endl;
            }
            else if (sharedBeltData->itemsOnBelt.front() == escargotValue)
            {
                cout << consumerName << " consumed: escargot sucker." << endl;
            }

            // If frog was consumed, update that semaphore as well
            if (sharedBeltData->itemsOnBelt.front() == frogValue)
            {
                sem_post(&sharedBeltData->availableFrogTreatSpaces);
            }

            sharedBeltData->itemsOnBelt.pop();
        }

        // There is more room to produce items, so update semaphore
        sem_post(&sharedBeltData->availableTreatSpaces);

        // Release the hold on the critical area
        sem_post(&sharedBeltData->mutex);

        // Sleep for time to consume next item
        nanosleep(&sleepTime, NULL);
    }
}
