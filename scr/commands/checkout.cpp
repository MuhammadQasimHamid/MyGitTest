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
                cout << FlattenfilePath;
                if(StagingIndex::isTrackedFile(FlattenfilePath) )
                {   
                    Cmp_Status cmpStatus =  Repository::WorkingDirCommitComp(FlattenfilePath,tE.hash);
                    if(cmpStatus == CMP_DIFFER ) // if the file is diff in WR or not Exist in WR
                    {
                        cout <<  "         Contents Differ";
                        string blobRawFileContents = readFileWithStoredObjectHash(tE.hash);
                        BlobObject bObj(blobRawFileContents); // deserilized constructor called
                        writeFile(FlattenfilePath,bObj.contents); // Re-write
                    }
                    else if(cmpStatus == CMP_WR_C_NotExist_WR)
                    {
                        cout <<  "         Write file";
                        string blobRawFileContents = readFileWithStoredObjectHash(tE.hash);
                            BlobObject bObj(blobRawFileContents); // deserilized constructor called
                            writeFile(FlattenfilePath,bObj.contents); // Re-write
                    }
                    else if(cmpStatus == CMP_SAME)
                    {
                        //do nothing
                    }
                    else if(cmpStatus == CMP_WR_C_NotExist_C) // so in commit, it does'nt exist so del from here
                    {
                        cout << "Delete file";
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
