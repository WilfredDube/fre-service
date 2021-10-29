#pragma once

#include <storage_credential.h>
#include <storage_account.h>
#include <blob/blob_client.h>

#include <string>
#include <sstream>

class CloudStorage
{
private:
    const std::string containerName = "fxtcadfiles";
    std::shared_ptr<azure::storage_lite::blob_client> blobClient = nullptr;

public:
    CloudStorage();

    std::string downloadBlob(std::string blobName);
};
