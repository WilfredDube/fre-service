#include "../include/cloud.h"

CloudStorage::CloudStorage()
{
    using namespace azure::storage_lite;

    std::string account_name = std::getenv("AZURE_BLOB_STORAGE_NAME");
    std::string account_key = std::getenv("AZURE_BLOB_STORAGE_KEY");

    std::shared_ptr<storage_credential> cred = std::make_shared<shared_key_credential>(account_name, account_key);
    std::shared_ptr<storage_account> account = std::make_shared<storage_account>(account_name, cred, /* use_https */ true);

    blobClient = std::make_shared<blob_client>(account, 16);
}

std::string CloudStorage::downloadBlob(std::string blobName)
{
    std::cout << "Creating ostringStream" << std::endl;
    std::ostringstream out_stream;
    std::cout << "Now downloading blob" << std::endl;
    auto ret = blobClient->download_blob_to_stream(containerName, blobName, 0, 0, out_stream).get();
    std::cout << "Finished downloading blob" << std::endl;
    if (!ret.success())
    {
        std::cout << "Failed to download blob, Error: " << ret.error().code << ", " << ret.error().code_name << std::endl;
        return nullptr;
    }

    std::cout << "Returning blob" << std::endl;
    return out_stream.str();
}
