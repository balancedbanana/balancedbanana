#pragma once

#include "IGateway.h"
#include "worker_details.h"
#include <cinttypes>
#include <vector>

namespace balancedbanana::database {
        class WorkerGateway : virtual public IGateway {
        public:
            explicit WorkerGateway(std::shared_ptr<QSqlDatabase> db);

            /**
             * Adds a worker to the database.
             * @param details  The worker to be added
             * @return The id of the worker.
             */
            uint64_t addWorker(const worker_details& details);

            /**
             * Deletes a worker with the given id from the database.
             * @param id  The id of the worker to be deleted.
             * @return True if the operation was successful, otherwise false
             */
            void removeWorker(uint64_t id);

            /**
             * Getter method for the information of a worker with the given id.
             * @param id  The id of the worker.
             * @return The details of the worker.
             */
            worker_details getWorker(uint64_t id);

            /**
             * Getter for all the workers in the database.
             * @return  Vector of all the workers in the database.
             */
            std::vector<worker_details> getWorkers();


            /**
             * Getter for a worker with a specific name.
             * @param name The name of the worker.
             * @return Returns the correct details of the worker if found, otherwise returns empty details struct with
             * invalid id.
             */
            worker_details getWorkerByName(const std::string& name);

            /**
             * Updates certain fields of the Worker in the DB with id = worker.id.
             * @param worker The Worker's new information.
             */
            void updateWorker(const worker_details& worker);
        };
    }