#include "commands/checkout.h"
#include "core/gitObject.h"
#include "core/repository.h"
#include "core/StagingIndex.h"
#include "utils/fileCRUD.h"
#include <iostream>
using namespace std;

void checkoutCommandExe(int argc, char *argv[])
{
    // pit checkout 67efef23fef23ef2f3ef23fe23e32f4
    if (argc == 3)
    {   
        // compare working-head
        string inputHash = argv[3-1];
        string rawFileContents = readFileWithStoredObjectHash(inputHash);
        GitObject gObj(rawFileContents); // deserilze constructor called
        if(gObj.objectType == Commit)
        {
            CommitObject cObj(rawFileContents);
            string treeRawFileContents = readFileWithStoredObjectHash(cObj.treeHash);
            TreeObject tObj(treeRawFileContents);
            map<path,treeEntry> flattenTree = Repository::FlattenTreeObject(tObj);
            for(auto it:flattenTree)
            {
                path FlattenfilePath = it.first;
                treeEntry tE = it.second;
                if(StagingIndex::isTrackedFile(FlattenfilePath) )
                {
                    FileStatus fStatus =  Repository::WorkingDirCommitComp(FlattenfilePath,tE.hash);
                    if(fStatus == File_ContentsDiffer)
                    {
                        string blobRawFileContents = readFileWithStoredObjectHash(tE.hash);
                        writeFile(FlattenfilePath,blobRawFileContents); // Re-write
                    }
                    else if(fStatus == File_NotExist)
                    {
                        string blobRawFileContents = readFileWithStoredObjectHash(tE.hash);
                        writeFile(FlattenfilePath,blobRawFileContents); // Re-write
                    }
                    else if(fStatus != File_Nothing)
                    {
                        deleteFile(FlattenfilePath);
                    }

                }
            }

        }
        else
        {
            cout << "Hash doesn't represent a commit to checkout." << endl;
        }
    }
    else
    {
        cout << "Invalid Params for checkout command." << endl;
    }
}
