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
        string inputHash = argv[3 - 1];
        // compare working-head

        if(!storedObjectExistsWithHash(inputHash))
        {
            // checkout branch
            // pit checkout dev
            if(exists(Repository::refsHeadFolderPath/inputHash))
            {
                Repository::setHEAD(inputHash); // Detach head not handled
            }
            else
            {
                cout << "error: pathspec '" << inputHash << "' did not match any file(s) known to pit." << endl;
                return;
            }
        }
        else
        {
            if (Repository::stagedChangesExist() || Repository::unstagedChangesExist())
            {

                string rawFileContents = readFileWithStoredObjectHash(inputHash);
                GitObject gObj(rawFileContents); // deserilze constructor called
                if (gObj.objectType == Commit)
                {
                    CommitObject cObj(rawFileContents);
                    string treeRawFileContents = readFileWithStoredObjectHash(cObj.treeHash);
                    TreeObject tObj(treeRawFileContents);
                    map<path, treeEntry> flattenTree = Repository::FlattenTreeObject(tObj);
                    for (auto it : flattenTree)
                    {
                        path FlattenfilePath = it.first;
                        treeEntry tE = it.second;
                        cout << FlattenfilePath;
                        if (StagingIndex::isTrackedFile(FlattenfilePath))
                        {
                            Cmp_Status cmpStatus = Repository::WorkingDirCommitComp(FlattenfilePath, tE.hash);
                            if (cmpStatus == CMP_DIFFER) // if the file is diff in WR or not Exist in WR
                            {
                                cout << "         Contents Differ";
                                string blobRawFileContents = readFileWithStoredObjectHash(tE.hash);
                                BlobObject bObj(blobRawFileContents);      // deserilized constructor called
                                writeFile(FlattenfilePath, bObj.contents); // Re-write
                            }
                            else if (cmpStatus == CMP_WR_C_NotExist_WR)
                            {
                                cout << "         Write file";
                                string blobRawFileContents = readFileWithStoredObjectHash(tE.hash);
                                BlobObject bObj(blobRawFileContents);      // deserilized constructor called
                                writeFile(FlattenfilePath, bObj.contents); // Re-write
                            }
                            else if (cmpStatus == CMP_SAME)
                            {
                                // do nothing
                            }
                            else if (cmpStatus == CMP_WR_C_NotExist_C) // so in commit, it does'nt exist so del from here
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
                cout << "error:Your local changes to the following files would be overwritten by checkout :" << endl;
                cout << "(use \" git status \" to check these local changes) " << endl;
                cout << "Please Commit your changes before checkout." << endl;
                cout << "Aborting" << endl;
            }
        }
    }
    else
    {
        cout << "Invalid Params for checkout command." << endl;
    }
}
